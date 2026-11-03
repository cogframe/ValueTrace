import shutil
import subprocess
import unittest
from pathlib import Path

from valuetrace.core import Program, analyze


class AnalysisTests(unittest.TestCase):
    def test_direct_calls_and_truncation(self):
        program = Program.from_text("""
define void @source(i32 %input) {
entry:
  call void @convert(i32 %input)
  ret void
}
define void @convert(i32 %count) {
entry:
  %large = mul i32 %count, 4
  %small = trunc i32 %large to i16
  call void @consume(i16 %small)
  ret void
}
""")
        finding = analyze(program, "source", "consume", 0, "ugt", 4096, 2)
        self.assertEqual(finding.status, "SAT")
        self.assertEqual(finding.call_chain, ("source", "convert", "consume"))
        self.assertEqual((finding.inputs["source.input"] * 4) % 65536, finding.target_value)
        self.assertGreater(finding.target_value, 4096)

    def test_return_value_from_called_function(self):
        program = Program.from_text("""
define i16 @convert(i32 %count) {
entry:
  %small = trunc i32 %count to i16
  ret i16 %small
}
define void @source(i32 %input) {
entry:
  %result = call i16 @convert(i32 %input)
  call void @consume(i16 %result)
  ret void
}
""")
        finding = analyze(program, "source", "consume", 0, "eq", 50000, 2)
        self.assertEqual(finding.status, "SAT")
        self.assertEqual(finding.inputs["source.input"] % 65536, 50000)

    def test_unsat_for_modeled_constant(self):
        program = Program.from_text("""
define void @source(i32 %input) {
entry:
  call void @consume(i16 2)
  ret void
}
""")
        finding = analyze(program, "source", "consume", 0, "eq", 3, 1)
        self.assertEqual(finding.status, "UNSAT")

    def test_depth_limit_is_unknown(self):
        program = Program.from_text("""
define void @source(i32 %x) {
entry:
  call void @middle(i32 %x)
  ret void
}
define void @middle(i32 %x) {
entry:
  call void @consume(i32 %x)
  ret void
}
""")
        finding = analyze(program, "source", "consume", 0, "eq", 3, 1)
        self.assertEqual(finding.status, "UNKNOWN")

    def test_branch_is_unknown(self):
        program = Program.from_text("""
define void @source(i32 %x) {
entry:
  br label %next
next:
  call void @consume(i32 %x)
  ret void
}
""")
        finding = analyze(program, "source", "consume", 0, "eq", 3, 1)
        self.assertEqual(finding.status, "UNKNOWN")

    def test_poison_arithmetic_is_unknown(self):
        program = Program.from_text("""
define void @source(i32 %x) {
entry:
  %next = add nsw i32 %x, 1
  call void @consume(i32 %next)
  ret void
}
""")
        finding = analyze(program, "source", "consume", 0, "eq", 3, 1)
        self.assertEqual(finding.status, "UNKNOWN")

    def test_variable_shift_is_unknown(self):
        program = Program.from_text("""
define void @source(i32 %x, i32 %amount) {
entry:
  %shifted = shl i32 %x, %amount
  call void @consume(i32 %shifted)
  ret void
}
""")
        finding = analyze(program, "source", "consume", 0, "eq", 3, 1)
        self.assertEqual(finding.status, "UNKNOWN")

    @unittest.skipUnless(shutil.which("clang++"), "clang++ is required")
    def test_real_clang_output(self):
        example = Path(__file__).resolve().parents[1] / "examples" / "chain.cpp"
        result = subprocess.run(
            ["clang++", "-S", "-emit-llvm", "-O1", "-fno-inline", "-fno-discard-value-names", str(example), "-o", "-"],
            check=True,
            capture_output=True,
            text=True,
        )
        program = Program.from_text(result.stdout)
        finding = analyze(program, "source", "consume", 0, "ugt", 4096, 2)
        self.assertEqual(finding.status, "SAT", finding.reason)
        self.assertEqual((finding.inputs["source.input"] * 4) % 65536, finding.target_value)


if __name__ == "__main__":
    unittest.main()
