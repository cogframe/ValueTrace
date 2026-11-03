"""Replay the frozen third- and fourth-round commands under both 6a semantics."""
from pathlib import Path
import importlib.util
import json

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
BASE = ROOT / "eval" / "juliet_run5a" / "run_eval.py"
spec = importlib.util.spec_from_file_location("run5a_replay", BASE)
replay = importlib.util.module_from_spec(spec)
spec.loader.exec_module(replay)

for round_number in (3, 4):
    replay.HERE = HERE / f"round{round_number}"
    replay.OLD = ROOT / "eval" / f"juliet_run{round_number}"
    replay.PREVIOUS = json.loads((replay.OLD / "parsed_results.json").read_text(encoding="utf-8"))
    replay.main()
