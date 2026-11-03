#include "valuetrace/ArithmeticReport.h"
#include "valuetrace/LlvmReader.h"
#include "valuetrace/RiskVerifier.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace valuetrace;
namespace {

void require(bool value, const std::string& message) {
    if (!value) throw std::runtime_error(message);
}

ImportedProgram read(const std::filesystem::path& file, const char* function) {
    ParameterQuery query;
    query.function = function;
    query.parameters = {0};
    return readParameterGraph(file, query);
}

Node find(const ImportedProgram& program, const char* label) {
    for (NodeId id = 0; id < program.graph.size(); ++id)
        if (program.graph.node(id).label == label) return program.graph.node(id);
    throw std::runtime_error(std::string("node not found: ") + label);
}

bool has(const Node& node, ArithmeticRiskReason reason) {
    for (const auto& risk : node.arithmeticRisks) if (risk.reason == reason) return true;
    return false;
}

std::string report(const ImportedProgram& program) {
    std::ostringstream output;
    const auto slice = buildSlice(program.graph, program.query);
    writeArithmeticReport(output, program.graph, slice, ReportFilter::forLevel(5));
    return output.str();
}

std::string report(const RiskVerificationReport& verified) {
    std::ostringstream output;
    writeArithmeticReport(output, verified.program.graph, verified.slice, ReportFilter::forLevel(5));
    return output.str();
}

} // namespace

int main(int argc, char** argv) {
    try {
        require(argc == 4, "expected debug IR, no-debug IR and conflict IR");
        const auto unsignedDebug = read(argv[1], "unsigned_sub");
        const auto& unsignedNode = find(unsignedDebug, "%sub");
        require(unsignedNode.integerType.sourceKind == SourceIntegerKind::Unsigned &&
                unsignedNode.integerType.basis.find("DW_ATE_unsigned") != std::string::npos &&
                has(unsignedNode, ArithmeticRiskReason::UnsignedUnderflow) &&
                !has(unsignedNode, ArithmeticRiskReason::SignedOverflow),
                "uint32_t subtraction was not classified using debug type");
        const auto& directReturn = find(read(argv[1], "direct_return_sub"), "%sub");
        require(directReturn.integerType.sourceKind == SourceIntegerKind::Unsigned &&
                directReturn.integerType.basis.find("debug-info operand dbg.value") != std::string::npos &&
                !has(directReturn, ArithmeticRiskReason::SignedOverflow),
                "direct return did not use matching operand debug types");
        const auto signedDebug = read(argv[1], "signed_sub");
        const auto& signedNode = find(signedDebug, "%sub");
        require(signedNode.integerType.sourceKind == SourceIntegerKind::Signed &&
                has(signedNode, ArithmeticRiskReason::SignedOverflow) &&
                !has(signedNode, ArithmeticRiskReason::UnsignedUnderflow),
                "int32_t subtraction was not classified using debug type");
        const auto& mixed = find(read(argv[1], "mixed_signed_result"), "%sub");
        require(mixed.integerType.conflict.find("operand source type signed") != std::string::npos &&
                has(mixed, ArithmeticRiskReason::SignedOverflow) &&
                has(mixed, ArithmeticRiskReason::UnsignedUnderflow),
                "source result type hid an opposing operand type");
        require(report(unsignedDebug).find("source-type=unsigned") != std::string::npos &&
                report(unsignedDebug).find("Type: source=unsigned resolved=unsigned basis=debug-info dbg.value") != std::string::npos,
                "debug type basis missing from warning report");

        const auto unsignedNoDebug = read(argv[2], "unsigned_sub");
        const auto& unknown = find(unsignedNoDebug, "%sub");
        require(unknown.integerType.sourceKind == SourceIntegerKind::Unknown &&
                unknown.integerType.basis.find("debug info missing") != std::string::npos &&
                has(unknown, ArithmeticRiskReason::UnsignedUnderflow) &&
                has(unknown, ArithmeticRiskReason::SignedOverflow),
                "no-debug build did not preserve the previous two-interpretation check");
        require(report(unsignedNoDebug).find("assumption=unsigned") != std::string::npos &&
                report(unsignedNoDebug).find("Type: source=unknown resolved=unknown") != std::string::npos,
                "no-debug report did not label the source type unknown");

        const auto& alias = find(read(argv[1], "alias_sub"), "%sub");
        require(alias.integerType.basis.find("CountAlias -> CountBase -> uint32_t") != std::string::npos &&
                alias.integerType.sourceKind == SourceIntegerKind::Unsigned, "typedef chain was not traversed");
        const auto& size = find(read(argv[1], "size_sub"), "%sub");
        require(size.bitWidth == 64 && size.integerType.basis.find("size_t") != std::string::npos &&
                size.integerType.sourceKind == SourceIntegerKind::Unsigned, "size_t signedness was not recovered");
        const auto& signedChar = find(read(argv[1], "char_value"), "arg0 %x");
        const auto& unsignedChar = find(read(argv[1], "unsigned_char_value"), "arg0 %x");
        require(signedChar.integerType.sourceKind == SourceIntegerKind::Signed &&
                signedChar.integerType.basis.find("DW_ATE_signed_char") != std::string::npos &&
                unsignedChar.integerType.sourceKind == SourceIntegerKind::Unsigned &&
                unsignedChar.integerType.basis.find("DW_ATE_unsigned_char") != std::string::npos,
                "char encodings were lost");
        const auto& boolean = find(read(argv[1], "bool_value"), "%storedv");
        require(boolean.integerType.sourceKind == SourceIntegerKind::Boolean &&
                boolean.integerType.basis.find("DW_ATE_boolean") != std::string::npos,
                "boolean encoding was not recovered");
        const auto& promoted16 = find(read(argv[1], "promoted_u16_sum"), "%add");
        require(promoted16.bitWidth == 32 && promoted16.integerType.sourceKind == SourceIntegerKind::Signed &&
                promoted16.arithmeticRisks.empty(),
                "uint16_t addition was misclassified as a 16-bit unsigned wrap");
        const auto narrowed = read(argv[1], "narrowed_u8_sum");
        const auto& promoted8 = find(narrowed, "%add");
        const auto& trunc8 = find(narrowed, "%conv2");
        require(promoted8.bitWidth == 32 && promoted8.integerType.sourceKind == SourceIntegerKind::Unknown &&
                promoted8.arithmeticRisks.empty() && trunc8.bitWidth == 8 &&
                has(trunc8, ArithmeticRiskReason::UnsignedTruncationLoss) &&
                !has(trunc8, ArithmeticRiskReason::SignedTruncationLoss),
                "uint8_t assignment loss must be reported at trunc, not promoted add");
        const auto& noDebugTemporary = find(read(argv[2], "narrowed_u8_sum"), "%add");
        require(noDebugTemporary.integerType.sourceKind == SourceIntegerKind::Unknown &&
                noDebugTemporary.integerType.basis.find("debug info missing") != std::string::npos,
                "unmapped intermediate value inherited a variable type without evidence");
        for (const auto* function : {"promoted_char_sum", "promoted_bool_sum"}) {
            const auto& promoted = find(read(argv[1], function), "%add");
            require(promoted.bitWidth == 32 && promoted.integerType.sourceKind == SourceIntegerKind::Signed &&
                    promoted.arithmeticRisks.empty(),
                    std::string(function) + " was not checked as promoted int arithmetic");
        }
        const auto& flaggedPromotion = find(read(argv[3], "promoted_u16_nsw"), "%r");
        require(flaggedPromotion.bitWidth == 32 && flaggedPromotion.integerType.sourceKind == SourceIntegerKind::Unknown &&
                flaggedPromotion.arithmeticRisks.empty(),
                "nsw on widened uint16_t operands produced an impossible wrap warning");
        const auto& enumeration = find(read(argv[1], "enum_value"), "arg0 %x");
        require(enumeration.integerType.sourceKind == SourceIntegerKind::Unknown &&
                enumeration.integerType.basis.find("enum underlying type is not inferred") != std::string::npos,
                "enum must be explicitly marked type unknown");

        const auto& attribute = find(read(argv[3], "attribute_only"), "%r");
        require(attribute.integerType.sourceKind == SourceIntegerKind::Unknown &&
                attribute.integerType.kind == SourceIntegerKind::Signed &&
                attribute.integerType.basis.find("signext") != std::string::npos &&
                has(attribute, ArithmeticRiskReason::SignedOverflow) &&
                !has(attribute, ArithmeticRiskReason::UnsignedUnderflow),
                "signext attribute did not supply lower-priority evidence");
        require(report(read(argv[3], "attribute_only")).find("attribute-type=signed") != std::string::npos,
                "ABI extension evidence was not labeled in the report");
        const auto& zeroAttribute = find(read(argv[3], "attribute_zero_only"), "%r");
        require(zeroAttribute.integerType.sourceKind == SourceIntegerKind::Unknown &&
                zeroAttribute.integerType.kind == SourceIntegerKind::Unsigned &&
                zeroAttribute.integerType.basis.find("zeroext") != std::string::npos &&
                has(zeroAttribute, ArithmeticRiskReason::UnsignedUnderflow) &&
                !has(zeroAttribute, ArithmeticRiskReason::SignedOverflow),
                "zeroext attribute did not supply lower-priority evidence");
        const auto& qualified = find(read(argv[3], "qualified"), "%r");
        require(qualified.integerType.sourceKind == SourceIntegerKind::Unsigned &&
                qualified.integerType.basis.find("volatile -> const -> NamedCount") != std::string::npos,
                "const/volatile typedef wrappers were not traversed");
        const auto& bitfield = find(read(argv[3], "bitfield_direct"), "%r");
        require(bitfield.integerType.sourceKind == SourceIntegerKind::Unknown &&
                bitfield.integerType.basis.find("bit-field") != std::string::npos,
                "bit-field must be explicitly marked type unknown");
        const auto conflict = read(argv[3], "conflicting");
        const auto& incompatible = find(conflict, "%r");
        require(incompatible.integerType.sourceKind == SourceIntegerKind::Unsigned &&
                incompatible.integerType.conflict.find("sdiv") != std::string::npos &&
                report(conflict).find("Type conflict:") != std::string::npos,
                "debug type and signed instruction conflict was silent");

        ParameterQuery verificationQuery;
        verificationQuery.function = "unsigned_sub";
        verificationQuery.parameters = {0};
        const auto verified = verifyArithmeticRisks(argv[1], verificationQuery);
        require(verified.candidates == 1, "source type did not narrow unsigned subtraction to one risk class");
        for (NodeId id = 0; id < verified.program.graph.size(); ++id)
            for (const auto& risk : verified.program.graph.node(id).arithmeticRisks)
                require(risk.status != ReportStatus::SourceConfirmed,
                        "debug source type incorrectly established source closure");
        verificationQuery.function = "signed_sub";
        const auto verifiedSigned = verifyArithmeticRisks(argv[1], verificationQuery);
        require(report(verifiedSigned).find("source signed overflow remains a candidate") != std::string::npos,
                "LLVM bit-pattern wording obscured the source signed-overflow risk");

        std::cout << "Source type evidence and risk classifications passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
