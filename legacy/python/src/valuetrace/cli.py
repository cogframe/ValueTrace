from __future__ import annotations

import argparse
from pathlib import Path
import sys

from .core import Program, UnsupportedIR, analyze


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(prog="valuetrace")
    sub = parser.add_subparsers(dest="command", required=True)
    command = sub.add_parser("analyze", help="analyze integer propagation in one LLVM IR file")
    command.add_argument("ir", type=Path)
    command.add_argument("--entry", required=True, help="entry function's LLVM symbol")
    command.add_argument("--target", required=True, help="callee:zero-based parameter index")
    command.add_argument("--op", choices=["eq", "ugt", "uge", "ult", "ule"], required=True)
    command.add_argument("--value", type=lambda text: int(text, 0), required=True)
    command.add_argument("--max-depth", type=int, default=3)
    command.add_argument("--timeout-ms", type=int, default=5000)
    args = parser.parse_args(argv)
    try:
        target, index_text = args.target.rsplit(":", 1)
        index = int(index_text)
        if index < 0:
            raise ValueError("target parameter index must be nonnegative")
        program = Program.from_text(args.ir.read_text(encoding="utf-8"))
        finding = analyze(program, args.entry, target, index, args.op, args.value, args.max_depth, args.timeout_ms)
    except (OSError, UnsupportedIR, ValueError) as exc:
        print(f"UNKNOWN: {exc}", file=sys.stderr)
        return 2
    print(finding.status)
    if finding.call_chain:
        print("Call chain: " + " -> ".join(finding.call_chain))
    if finding.inputs is not None:
        for name, value in finding.inputs.items():
            print(f"Entry parameter {name} = {value} (0x{value:x})")
    if finding.target_value is not None:
        print(f"Target parameter = {finding.target_value} (0x{finding.target_value:x})")
        print(f"Target expression: {finding.expression}")
    if finding.reason:
        print(f"Reason: {finding.reason}")
    return 0 if finding.status == "SAT" else 1


if __name__ == "__main__":
    raise SystemExit(main())
