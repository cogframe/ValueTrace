"""Small, explicit LLVM IR subset used by the first ValueTrace prototype.

This is intentionally a strict reader, not a general LLVM IR parser. Unknown
constructs become UNKNOWN results instead of silently losing dependencies.
"""

from __future__ import annotations

from dataclasses import dataclass
import re

import z3


class UnsupportedIR(Exception):
    pass


_SYMBOL = r"[A-Za-z_.$][\w.$-]*"
_DEFINE = re.compile(rf"^define\b.*?@({_SYMBOL})\(([^)]*)\).*\{{$")
_CALL = re.compile(rf"\bcall\b.*?\b(void|i\d+)\s+@({_SYMBOL})\(([^)]*)\)")
_PARAM = re.compile(r"\bi(\d+)\b.*?(%[\w.$-]+)\s*$")
_TYPED_VALUE = re.compile(r"\bi(\d+)\b(?:\s+\w+)*\s+(%[\w.$-]+|-?\d+)\s*$")
_BINARY = re.compile(
    r"^(add|sub|mul|shl|lshr|ashr|and|or|xor)\s+((?:(?:nsw|nuw|exact)\s+)*)i(\d+)\s+([^,]+),\s*(\S+)$"
)
_CAST = re.compile(r"^(trunc|zext|sext)\s+i(\d+)\s+(\S+)\s+to\s+i(\d+)$")
_RETURN = re.compile(r"^ret\s+i(\d+)\s+(\S+)$")


@dataclass(frozen=True)
class Parameter:
    name: str
    width: int


@dataclass
class Function:
    name: str
    params: list[Parameter]
    instructions: list[str]

    def assignments(self) -> dict[str, str]:
        result = {}
        for line in self.instructions:
            if line.startswith("%") and " = " in line:
                name, rhs = line.split(" = ", 1)
                result[name] = rhs
        return result


@dataclass(frozen=True)
class Call:
    callee: str
    arguments: list[tuple[int, str]]
    result_width: int | None


@dataclass
class Program:
    functions: dict[str, Function]

    @classmethod
    def from_text(cls, text: str) -> Program:
        functions = {}
        current_name = None
        current_params: list[Parameter] = []
        lines: list[str] = []
        for raw in text.splitlines():
            line = raw.strip()
            if current_name is None:
                match = _DEFINE.match(line)
                if not match:
                    continue
                current_name = match.group(1)
                current_params = []
                for part in match.group(2).split(","):
                    part = part.strip()
                    if not part:
                        continue
                    param = _PARAM.search(part)
                    if param is None:
                        raise UnsupportedIR(f"{current_name}: unsupported parameter: {part}")
                    current_params.append(Parameter(param.group(2), int(param.group(1))))
                lines = []
            elif line == "}":
                functions[current_name] = Function(current_name, current_params, lines)
                current_name = None
            elif line and not line.startswith(";"):
                lines.append(line.split(" ; ", 1)[0])
        if current_name is not None:
            raise UnsupportedIR(f"unterminated function: {current_name}")
        return cls(functions)


def parse_call(line: str) -> Call | None:
    match = _CALL.search(line)
    if not match:
        if re.search(r"\bcall\b", line):
            raise UnsupportedIR(f"indirect or unsupported call: {line}")
        return None
    arguments = []
    for part in match.group(3).split(","):
        part = part.strip()
        if not part:
            continue
        value = _TYPED_VALUE.search(part)
        if value is None:
            raise UnsupportedIR(f"unsupported call argument: {part}")
        arguments.append((int(value.group(1)), value.group(2)))
    result_width = None if match.group(1) == "void" else int(match.group(1)[1:])
    return Call(match.group(2), arguments, result_width)


@dataclass
class Finding:
    status: str
    reason: str = ""
    call_chain: tuple[str, ...] = ()
    target_value: int | None = None
    inputs: dict[str, int] | None = None
    expression: str = ""


class Frame:
    def __init__(
        self,
        program: Program,
        function: Function,
        arguments: list[z3.BitVecRef],
        depth: int,
        max_depth: int,
        active: tuple[str, ...],
    ) -> None:
        if len(arguments) != len(function.params):
            raise UnsupportedIR(f"{function.name}: argument count mismatch")
        self.program = program
        self.function = function
        self.depth = depth
        self.max_depth = max_depth
        self.active = active
        self.values = {p.name: arg for p, arg in zip(function.params, arguments)}
        self.assignments = function.assignments()
        self.busy: set[str] = set()
        blocks = [line for line in function.instructions if line.endswith(":")]
        if len(blocks) != 1 or any(
            re.match(r"^(br|switch|indirectbr|invoke|callbr)\b", line)
            for line in function.instructions
        ):
            raise UnsupportedIR(f"{function.name}: only a single straight-line block is supported")
        if any(re.search(r"\b(load|store|atomicrmw|cmpxchg)\b", line) for line in function.instructions):
            raise UnsupportedIR(f"{function.name}: memory operations are not supported")

    def operand(self, token: str, width: int) -> z3.BitVecRef:
        token = token.strip()
        if re.fullmatch(r"-?\d+", token):
            return z3.BitVecVal(int(token), width)
        if token not in self.values:
            if token not in self.assignments:
                raise UnsupportedIR(f"{self.function.name}: unknown value {token}")
            if token in self.busy:
                raise UnsupportedIR(f"{self.function.name}: cyclic value {token}")
            self.busy.add(token)
            try:
                self.values[token] = self.evaluate(self.assignments[token])
            finally:
                self.busy.remove(token)
        result = self.values[token]
        if result.size() != width:
            raise UnsupportedIR(f"{self.function.name}: bit width mismatch for {token}")
        return result

    def evaluate(self, rhs: str) -> z3.BitVecRef:
        binary = _BINARY.match(rhs)
        if binary:
            operation, flags, width, left, right = binary.groups()
            if flags:
                raise UnsupportedIR(f"{self.function.name}: poison-producing arithmetic flags: {rhs}")
            width = int(width)
            if operation in {"shl", "lshr", "ashr"}:
                if not re.fullmatch(r"\d+", right) or int(right) >= width:
                    raise UnsupportedIR(f"{self.function.name}: variable or invalid shift amount: {rhs}")
            a = self.operand(left, width)
            b = self.operand(right, width)
            operations = {
                "add": lambda: a + b,
                "sub": lambda: a - b,
                "mul": lambda: a * b,
                "shl": lambda: a << b,
                "lshr": lambda: z3.LShR(a, b),
                "ashr": lambda: a >> b,
                "and": lambda: a & b,
                "or": lambda: a | b,
                "xor": lambda: a ^ b,
            }
            return operations[operation]()
        cast = _CAST.match(rhs)
        if cast:
            operation, source_width, source, dest_width = cast.groups()
            source_width, dest_width = int(source_width), int(dest_width)
            value = self.operand(source, source_width)
            if operation == "trunc" and dest_width < source_width:
                return z3.Extract(dest_width - 1, 0, value)
            if operation == "zext" and dest_width > source_width:
                return z3.ZeroExt(dest_width - source_width, value)
            if operation == "sext" and dest_width > source_width:
                return z3.SignExt(dest_width - source_width, value)
            raise UnsupportedIR(f"{self.function.name}: invalid cast: {rhs}")
        call = parse_call(rhs)
        if call:
            if call.result_width is None:
                raise UnsupportedIR(f"{self.function.name}: void call used as value")
            if self.depth >= self.max_depth:
                raise UnsupportedIR("call depth limit reached while evaluating a return value")
            callee = self.program.functions.get(call.callee)
            if callee is None or call.callee in self.active:
                raise UnsupportedIR(f"{self.function.name}: external or recursive value call: {call.callee}")
            args = [self.operand(value, width) for width, value in call.arguments]
            frame = Frame(self.program, callee, args, self.depth + 1, self.max_depth, self.active + (call.callee,))
            return frame.return_value(call.result_width)
        raise UnsupportedIR(f"{self.function.name}: unsupported expression: {rhs}")

    def return_value(self, expected_width: int) -> z3.BitVecRef:
        returns = [_RETURN.match(line) for line in self.function.instructions]
        returns = [match for match in returns if match]
        if len(returns) != 1 or int(returns[0].group(1)) != expected_width:
            raise UnsupportedIR(f"{self.function.name}: unsupported return")
        return self.operand(returns[0].group(2), expected_width)


def _comparison(expression: z3.BitVecRef, op: str, value: int) -> z3.BoolRef:
    rhs = z3.BitVecVal(value, expression.size())
    return {
        "eq": lambda: expression == rhs,
        "ugt": lambda: z3.UGT(expression, rhs),
        "uge": lambda: z3.UGE(expression, rhs),
        "ult": lambda: z3.ULT(expression, rhs),
        "ule": lambda: z3.ULE(expression, rhs),
    }[op]()


def analyze(
    program: Program,
    entry: str,
    target: str,
    parameter_index: int,
    op: str,
    value: int,
    max_depth: int,
    timeout_ms: int = 5000,
) -> Finding:
    if entry not in program.functions:
        raise ValueError(f"entry function not found: {entry}")
    if max_depth < 1:
        raise ValueError("max_depth must be at least 1")
    if value < 0:
        raise ValueError("target value must be nonnegative")
    if op not in {"eq", "ugt", "uge", "ult", "ule"}:
        raise ValueError(f"unknown comparison: {op}")

    root = program.functions[entry]
    inputs = {
        f"{entry}.{p.name[1:]}": z3.BitVec(f"{entry}.{p.name[1:]}", p.width)
        for p in root.params
    }
    candidates: list[tuple[tuple[str, ...], z3.BitVecRef]] = []
    problems: list[str] = []

    def walk(function: Function, args: list[z3.BitVecRef], chain: tuple[str, ...]) -> None:
        try:
            frame = Frame(program, function, args, len(chain) - 1, max_depth, chain)
            for line in function.instructions:
                call = parse_call(line)
                if call is None:
                    continue
                if call.callee == "llvm.assume":
                    raise UnsupportedIR(f"{function.name}: llvm.assume path constraints are not supported")
                if call.callee == target:
                    if parameter_index >= len(call.arguments):
                        raise UnsupportedIR(f"{target}: parameter {parameter_index} does not exist")
                    width, token = call.arguments[parameter_index]
                    expression = frame.operand(token, width)
                    candidates.append((chain + (target,), expression))
                    continue
                callee = program.functions.get(call.callee)
                if callee is None:
                    raise UnsupportedIR(f"{function.name}: external call not modeled: {call.callee}")
                if len(chain) >= max_depth:
                    problems.append(f"call depth limit reached at {function.name} -> {call.callee}")
                    continue
                if call.callee in chain:
                    problems.append(f"recursive call at {function.name} -> {call.callee}")
                    continue
                call_args = [frame.operand(token, width) for width, token in call.arguments]
                walk(callee, call_args, chain + (call.callee,))
        except UnsupportedIR as exc:
            problems.append(str(exc))

    walk(root, list(inputs.values()), (entry,))
    for chain, expression in candidates:
        if value >= 1 << expression.size():
            raise ValueError(f"target value {value} does not fit in {expression.size()} bits")
        solver = z3.Solver()
        solver.set(timeout=timeout_ms)
        solver.add(_comparison(expression, op, value))
        result = solver.check()
        if result == z3.sat:
            model = solver.model()
            return Finding(
                "SAT",
                call_chain=chain,
                target_value=model.eval(expression, model_completion=True).as_long(),
                inputs={name: model.eval(symbol, model_completion=True).as_long() for name, symbol in inputs.items()},
                expression=str(z3.simplify(expression)),
            )
        if result == z3.unknown:
            problems.append(f"Z3: {solver.reason_unknown()}")
    if problems:
        return Finding("UNKNOWN", reason="; ".join(dict.fromkeys(problems)))
    if not candidates:
        return Finding("UNKNOWN", reason=f"no direct call to {target} found within depth {max_depth}")
    return Finding("UNSAT", reason="no modeled call chain satisfies the condition")
