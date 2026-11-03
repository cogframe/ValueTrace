#include "valuetrace/PathSolver.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

using namespace valuetrace;

namespace {

void require(bool condition, const std::string& reason) {
    if (!condition) throw std::runtime_error(reason);
}

ReachabilityQuery query(std::string target, std::string op, std::string value, unsigned up = 1) {
    ReachabilityQuery result;
    result.target.function = std::move(target);
    result.target.parameters = {0};
    result.target.upDepth = up;
    result.comparison = std::move(op);
    result.value = std::move(value);
    return result;
}

ReachabilityResult expect(const std::filesystem::path& file, const ReachabilityQuery& request, SolveStatus status) {
    auto result = solveParameterReachability(file, request);
    if (result.status != status) {
        std::string notes;
        for (const auto& note : result.diagnostics) notes += "\n" + note;
        throw std::runtime_error("wrong result for " + request.target.function + " " + request.comparison + " " + request.value + ": " + result.reason + notes);
    }
    return result;
}

void checkCpp(const std::filesystem::path& guarded, const std::filesystem::path& unguarded,
              const std::filesystem::path& bitcode) {
    auto request = query("C", "ugt", "4096", 2);
    request.target.parameters.clear();
    request.target.parameterNames = {"length"};
    const auto safe = expect(guarded, request, SolveStatus::Unsat);
    require(safe.candidatesChecked != 0, "guarded target was not checked");
    const auto witness = expect(unguarded, request, SolveStatus::Sat);
    require(witness.inputs.size() == 1 && !witness.path.empty() && !witness.constraints.empty(), "witness lacks input, path or constraints");
    const auto input = std::stoull(witness.inputs.front().unsignedValue);
    require(input > 1024 && input <= 0xffffffffULL && input * 4 == std::stoull(witness.targetValue), "witness does not match real C++ arithmetic");
    expect(bitcode, request, SolveStatus::Unsat);
    request.target.upDepth = 0;
    const auto boundary = expect(guarded, request, SolveStatus::Sat);
    require(boundary.upstreamBoundary, "missing caller-scope boundary note");
    request.target.upDepth = 2;
    request.maxSteps = 1;
    expect(unguarded, request, SolveStatus::Unknown);
}

void checkPaths(const std::filesystem::path& file) {
    expect(file, query("guard_sink", "eq", "11"), SolveStatus::Unsat);
    const auto guard = expect(file, query("guard_sink", "eq", "10"), SolveStatus::Sat);
    require(guard.inputs.front().unsignedValue == "9", "branch constraint not enforced");
    expect(file, query("phi_sink", "eq", "10"), SolveStatus::Unsat);
    expect(file, query("phi_sink", "eq", "20"), SolveStatus::Sat);
    expect(file, query("select_sink", "eq", "10"), SolveStatus::Unsat);
    expect(file, query("select_sink", "eq", "20"), SolveStatus::Sat);
    const auto switchCase = expect(file, query("switch_sink", "eq", "20"), SolveStatus::Sat);
    require(switchCase.inputs.front().unsignedValue == "2", "switch case constraint lost");
    expect(file, query("switch_sink", "eq", "15"), SolveStatus::Unsat);
    expect(file, query("switch_sink", "eq", "30"), SolveStatus::Sat);
    expect(file, query("nowrap_sink", "eq", "2147483648"), SolveStatus::Unsat);
    expect(file, query("wrap_sink", "eq", "2147483648"), SolveStatus::Sat);
    expect(file, query("unused_poison_sink", "eq", "7"), SolveStatus::Sat);
    expect(file, query("selected_poison_sink", "eq", "7"), SolveStatus::Unsat);
    expect(file, query("divide_zero_sink", "eq", "7"), SolveStatus::Unsat);
    expect(file, query("poison_divisor_sink", "eq", "7"), SolveStatus::Unsat);
    const auto phiSafe = expect(file, query("phi_poison_sink", "eq", "7"), SolveStatus::Sat);
    require(phiSafe.inputs.front().unsignedValue == "0", "unselected PHI poison contaminated the safe predecessor");
    expect(file, query("phi_poison_sink", "eq", "8"), SolveStatus::Unsat);
    const auto hasDiagnostic = [](const ReachabilityResult& result, const std::string& text) {
        return std::any_of(result.diagnostics.begin(), result.diagnostics.end(), [&](const auto& note) {
            return note.find(text) != std::string::npos && note.find("operand %bad") != std::string::npos;
        });
    };
    const auto poisonBranch = expect(file, query("poison_branch_sink", "eq", "7"), SolveStatus::Unsat);
    require(hasDiagnostic(poisonBranch, "Potential undefined behavior: poison used as branch condition"),
            "poison branch condition was not classified as undefined behavior");
    const auto poisonDivisor = expect(file, query("generated_poison_divisor_sink", "eq", "7"), SolveStatus::Unsat);
    require(hasDiagnostic(poisonDivisor, "Potential undefined behavior: poison used as division or remainder divisor"),
            "poison divisor was not classified as undefined behavior");
    expect(file, query("noreturn_sink", "eq", "7", 2), SolveStatus::Unsat);
    const auto signedCompare = expect(file, query("signed_cmp_sink", "eq", "11"), SolveStatus::Sat);
    require(signedCompare.inputs.front().signedValue.starts_with('-'), "i64 signed comparison treated as unsigned");
    expect(file, query("unsigned_cmp_sink", "eq", "11"), SolveStatus::Unsat);
    const auto cast = expect(file, query("cast_sink", "eq", "-1"), SolveStatus::Sat);
    require(cast.inputs.front().signedValue == "-1" && cast.targetValue == "18446744073709551615", "sext i32 -> i64 semantics lost");
    expect(file, query("memory_sink", "eq", "7"), SolveStatus::Unknown);
    expect(file, query("loop_sink", "ugt", "0"), SolveStatus::Unknown);
    expect(file, query("loop_sink", "eq", "0"), SolveStatus::Sat);
    expect(file, query("helper_sink", "eq", "7", 1), SolveStatus::Unknown);
    const auto helper = expect(file, query("helper_sink", "eq", "7", 2), SolveStatus::Sat);
    require(helper.inputs.front().unsignedValue == "6", "return value mapping lost");
    expect(file, query("context_sink", "eq", "9", 2), SolveStatus::Unsat);
    expect(file, query("context_sink", "eq", "5", 2), SolveStatus::Sat);
    bool rejected = false;
    try { solveParameterReachability(file, query("cast_sink", "eq", "0x10000000000000000")); }
    catch (const std::invalid_argument&) { rejected = true; }
    require(rejected, "out-of-width query literal must be rejected, not wrapped");
}

} // namespace

int main(int argc, char** argv) {
    try {
        if (argc != 5) throw std::invalid_argument("expected guarded ll, unguarded ll, guarded bc and path fixture");
        checkCpp(argv[1], argv[2], argv[3]);
        checkPaths(argv[4]);
        std::cout << "Path solver: branch/switch/phi/select, i32/i64, contexts, defined IR and unknown boundaries passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
