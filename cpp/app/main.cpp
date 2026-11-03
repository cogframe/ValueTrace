#include "valuetrace/Slice.h"
#include "valuetrace/ArithmeticReport.h"
#ifdef VALUETRACE_WITH_LLVM
#include "valuetrace/LlvmReader.h"
#include "valuetrace/ParameterSelection.h"
#include "SelectionMenu.h"
#include "SolveOutput.h"
#include "valuetrace/RiskVerifier.h"
#include "valuetrace/ProgramInput.h"
#endif

#include <charconv>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <set>
#include <optional>

namespace {

unsigned parseDepth(std::string_view text) {
    unsigned value = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size()) {
        throw std::invalid_argument("depth must be a nonnegative integer");
    }
    return value;
}

const std::string& functionName(const valuetrace::Node& node) {
    return node.displayFunction.empty() ? node.function : node.displayFunction;
}

void printSlice(const valuetrace::DependencyGraph& graph,
                std::string_view title, const valuetrace::DirectionSlice& slice) {
    std::cout << title << '\n';
    for (const auto& reached : slice.nodes) {
        const auto& node = graph.node(reached.node);
        std::cout << "  #" << reached.node << ' ' << functionName(node) << ':' << node.label
                  << " i" << node.bitWidth << " depth=" << reached.callDepth
                  << " context=" << node.context;
        const auto& op = node.operation;
        if (!op.opcode.empty()) std::cout << " opcode=" << op.opcode;
        if (!op.predicate.empty()) std::cout << " predicate=" << op.predicate;
        if (op.interpretation == valuetrace::IntegerInterpretation::Signed) std::cout << " interpretation=signed";
        if (op.interpretation == valuetrace::IntegerInterpretation::Unsigned) std::cout << " interpretation=unsigned";
        if (op.noSignedWrap) std::cout << " nsw";
        if (op.noUnsignedWrap) std::cout << " nuw";
        if (op.exact) std::cout << " exact";
        if (op.nonNegative) std::cout << " nneg";
        if (op.sameSign) std::cout << " samesign";
        if (op.disjoint) std::cout << " disjoint";
        if (!node.source.file.empty()) std::cout << " source=" << node.source.file << ':' << node.source.line;
        if (!node.unresolved.empty()) std::cout << " unresolved=" << node.unresolved;
        std::cout << '\n';
        if (!op.ir.empty()) std::cout << "    IR: " << op.ir << '\n';
    }
    std::set<valuetrace::NodeId> included;
    for (const auto& reached : slice.nodes) included.insert(reached.node);
    for (auto from : included) {
        for (auto edgeId : graph.outgoing(from)) {
            const auto& edge = graph.edge(edgeId);
            if (!included.contains(edge.to)) continue;
            std::cout << "    #" << from << " -> #" << edge.to
                      << " operand=" << edge.operandIndex;
            if (edge.kind == valuetrace::EdgeKind::CallBoundary) std::cout << " call=" << edge.callSite;
            std::cout << '\n';
        }
    }
    for (const auto& boundary : slice.boundaries) {
        const auto& node = graph.node(boundary.notExpanded);
        std::cout << "  depth boundary: " << functionName(node) << ':' << node.label
                  << " context=" << node.context << " call=" << graph.edge(boundary.edge).callSite << '\n';
    }
}

} // namespace

int main(int argc, char** argv) {
    try {
        if (argc == 1 || std::string_view(argv[1]) == "--help") {
            std::cout << "ValueTrace\n"
                         "Usage: valuetrace demo [--up N] [--down N]\n"
                         "       valuetrace trace FILE.ll|FILE.bc --function NAME --param-name SOURCE_NAME [--param-name NAME ...] [--up N] [--down N]\n"
                         "       Numeric fallback: trace FILE --function NAME --param LLVM_INDEX [--param LLVM_INDEX ...]\n"
                         "       valuetrace inspect FILE --function NAME\n"
                         "       valuetrace solve FILE --function NAME --param-name NAME --op eq|ugt|slt|... --value INTEGER [--up N]\n"
                         "       valuetrace verify FILE --function NAME --param-name NAME [--up N] [--down N] [--semantics ub-free|wrap] [--rand-max N|none]\n"
                         "       valuetrace verify FILE --return-of NAME [--up N] [--down N]\n"
                         "       All LLVM commands accept FILE [FILE ...], a directory, or repeated --input PATH.\n"
                         "       solve/verify limits: --timeout-ms 5000 --max-states 2000 --max-steps 50000\n"
                         "       trace also accepts --function-choice N and --param-choice N (1-based).\n"
                         "       trace/verify reports: --report-level 1..5 (default 2) OR --show-status LIST\n"
                         "       Statuses: source-confirmed,boundary-triggerable,unknown,unverified,scoped-unreachable; all/none.\n"
                         "       If choices are required and omitted, trace lists candidates and prompts.\n"
                         "trace reports unverified candidates; verify checks scoped arithmetic risks. No whole-program safety verdict.\n";
            return 0;
        }
#ifdef VALUETRACE_WITH_LLVM
        if (std::string_view(argv[1]) == "trace" || std::string_view(argv[1]) == "inspect" ||
            std::string_view(argv[1]) == "solve" || std::string_view(argv[1]) == "verify") {
            if (argc < 3) throw std::invalid_argument("missing LLVM input file");
            const bool inspect = std::string_view(argv[1]) == "inspect";
            const bool solve = std::string_view(argv[1]) == "solve";
            const bool verify = std::string_view(argv[1]) == "verify";
            valuetrace::ParameterQuery query;
            valuetrace::ReachabilityQuery reachability;
            if (solve) query.downDepth = 0;
            std::optional<unsigned> functionChoice;
            std::vector<unsigned> parameterChoices;
            std::optional<unsigned> reportLevel;
            std::optional<std::string> reportStatuses;
            std::vector<std::filesystem::path> inputs;
            for (int i = 2; i < argc;) {
                const std::string_view option = argv[i];
                if (!option.starts_with("--")) { inputs.emplace_back(argv[i++]); continue; }
                if (i + 1 >= argc) throw std::invalid_argument("missing option value");
                if (inspect && option != "--function" && option != "--input") throw std::invalid_argument("inspect accepts --function and --input only");
                if (option == "--input") inputs.emplace_back(argv[i + 1]);
                else if (option == "--function") query.function = argv[i + 1];
                else if (verify && option == "--return-of") {
                    query.function = argv[i + 1];
                    query.selectReturn = true;
                }
                else if (option == "--param") query.parameters.push_back(parseDepth(argv[i + 1]));
                else if (option == "--param-name") query.parameterNames.push_back(argv[i + 1]);
                else if (option == "--function-choice") functionChoice = parseDepth(argv[i + 1]);
                else if (option == "--param-choice") parameterChoices.push_back(parseDepth(argv[i + 1]));
                else if (option == "--up") query.upDepth = parseDepth(argv[i + 1]);
                else if (option == "--down") query.downDepth = parseDepth(argv[i + 1]);
                else if (option == "--report-level") {
                    if (solve) throw std::invalid_argument("--report-level filters trace/verify ArithmeticRisk reports; solve keeps its independent SAT/UNSAT/UNKNOWN result");
                    if (reportLevel) throw std::invalid_argument("--report-level may only be provided once");
                    unsigned level = 0;
                    const std::string_view text = argv[i + 1];
                    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), level);
                    if (error != std::errc{} || end != text.data() + text.size()) throw std::invalid_argument("--report-level must be an integer from 1 to 5");
                    valuetrace::ReportFilter::forLevel(level);
                    reportLevel = level;
                }
                else if (option == "--show-status") {
                    if (solve) throw std::invalid_argument("--show-status filters trace/verify ArithmeticRisk reports; solve keeps its independent SAT/UNSAT/UNKNOWN result");
                    if (reportStatuses) throw std::invalid_argument("--show-status may only be provided once");
                    valuetrace::ReportFilter::forStatuses(argv[i + 1]);
                    reportStatuses = argv[i + 1];
                }
                else if (solve && option == "--op") reachability.comparison = argv[i + 1];
                else if (solve && option == "--value") reachability.value = argv[i + 1];
                else if ((solve || verify) && option == "--timeout-ms") reachability.timeoutMs = parseDepth(argv[i + 1]);
                else if ((solve || verify) && option == "--max-states") reachability.maxStates = parseDepth(argv[i + 1]);
                else if ((solve || verify) && option == "--max-steps") reachability.maxSteps = parseDepth(argv[i + 1]);
                else if (verify && option == "--external-input-source") reachability.externalInputSources.push_back(argv[i + 1]);
                else if (verify && option == "--semantics") {
                    const std::string_view mode = argv[i + 1];
                    if (mode == "wrap") reachability.noWrapSemantics = valuetrace::NoWrapSemantics::Wrap;
                    else if (mode == "ub-free") reachability.noWrapSemantics = valuetrace::NoWrapSemantics::UbFree;
                    else throw std::invalid_argument("--semantics must be wrap or ub-free");
                }
                else if (verify && option == "--rand-max") {
                    if (std::string_view(argv[i + 1]) == "none") reachability.randUnbounded = true;
                    else reachability.randMaxOverride = parseDepth(argv[i + 1]);
                }
                else throw std::invalid_argument("unknown option");
                i += 2;
            }
            const auto files = valuetrace::resolveProgramInputs(inputs);
            std::cout << "LLVM inputs: " << files.size() << '\n';
            for (const auto& file : files) std::cout << "  " << file.generic_string() << '\n';
            if (files.size() > 1) std::cout << "Inputs must share a build configuration; IR checks do not establish macro/configuration consistency.\n";
            if (query.function.empty()) throw std::invalid_argument("--function is required");
            if (reportLevel && reportStatuses) throw std::invalid_argument("choose either --report-level or --show-status, not both");
            const auto reportFilter = reportStatuses ? valuetrace::ReportFilter::forStatuses(*reportStatuses) :
                valuetrace::ReportFilter::forLevel(reportLevel.value_or(2));
            const auto candidates = valuetrace::inspectParameters(files, query.function);
            if (inspect) {
                valuetrace::cli::printFunctions(candidates);
                for (const auto& candidate : candidates) valuetrace::cli::printParameters(candidate);
                return 0;
            }
            const auto& function = valuetrace::cli::chooseFunction(candidates, functionChoice);
            query.function = function.symbol;
            if (query.selectReturn && (!parameterChoices.empty() || !query.parameters.empty() || !query.parameterNames.empty()))
                throw std::invalid_argument("--return-of cannot be combined with parameter selectors");
            if (!query.selectReturn && (!parameterChoices.empty() || (query.parameters.empty() && query.parameterNames.empty()))) {
                valuetrace::cli::printParameters(function);
                if (parameterChoices.empty()) valuetrace::cli::chooseParameters(function, query);
                else for (auto choice : parameterChoices) valuetrace::cli::addParameterChoice(function, choice, query);
            }
            if (solve) {
                reachability.target = query;
                return valuetrace::cli::printSolveResult(valuetrace::solveParameterReachability(files, reachability));
            }
            valuetrace::ImportedProgram program;
            valuetrace::SliceResult slice;
            if (verify) {
                const valuetrace::VerificationOptions limits{reachability.timeoutMs, reachability.maxStates,
                    reachability.maxSteps, reachability.externalInputSources,
                    reachability.randMaxOverride, reachability.randUnbounded, reachability.noWrapSemantics};
                auto report = valuetrace::verifyArithmeticRisks(files, query, limits);
                std::cout << "Arithmetic verification: candidates=" << report.candidates << " solver-checks=" << report.solverChecks << '\n';
                program = std::move(report.program);
                slice = std::move(report.slice);
            } else {
                program = valuetrace::readParameterGraph(files, query);
                slice = valuetrace::buildSlice(program.graph, program.query);
            }
            std::cout << "Selected " << function.signature << ", parameter nodes=" << program.query.parameters.size()
                      << (verify ? ". Operation-specific, scoped warning verification; no whole-program safety conclusion.\n" :
                                   ". Value dependencies and unverified arithmetic warnings; no safety conclusion.\n");
            for (const auto& selection : program.selections) std::cout << "Mapping: " << selection << '\n';
            printSlice(program.graph, "Upstream:", slice.upstream);
            printSlice(program.graph, "Downstream:", slice.downstream);
            valuetrace::writeArithmeticReport(std::cout, program.graph, slice, reportFilter);
            for (const auto& diagnostic : program.diagnostics) std::cout << "Unresolved/model note: " << diagnostic << '\n';
            return 0;
        }
#endif
        if (std::string_view(argv[1]) != "demo") {
            throw std::invalid_argument("unknown command; use --help");
        }
        unsigned up = 1, down = 1;
        for (int i = 2; i < argc; i += 2) {
            if (i + 1 >= argc) throw std::invalid_argument("missing option value");
            const std::string_view option = argv[i];
            if (option == "--up") up = parseDepth(argv[i + 1]);
            else if (option == "--down") down = parseDepth(argv[i + 1]);
            else throw std::invalid_argument("unknown option");
        }

        using namespace valuetrace;
        DependencyGraph graph;
        const auto input = graph.addNode({"A", "input", 32, NodeKind::Parameter, "root"});
        const auto bytes = graph.addNode({"A", "input * 4", 32, NodeKind::Operation, "root"});
        const auto selected = graph.addNode({"B", "count", 32, NodeKind::Parameter, "A:call1"});
        const auto narrowed = graph.addNode({"B", "trunc to i16", 16, NodeKind::Operation, "A:call1"});
        const auto use = graph.addNode({"C", "length", 16, NodeKind::Parameter, "A:call1/B:call1"});
        graph.addEdge(input, bytes, EdgeKind::Local);
        graph.addEdge(bytes, selected, EdgeKind::CallBoundary, "A:call1");
        graph.addEdge(selected, narrowed, EdgeKind::Local);
        graph.addEdge(narrowed, use, EdgeKind::CallBoundary, "B:call1");

        const auto result = buildSlice(graph, {{selected}, up, down});
        std::cout << "Demo graph, selected B:count. No safety conclusion.\n";
        printSlice(graph, "Upstream:", result.upstream);
        printSlice(graph, "Downstream:", result.downstream);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 2;
    }
}
