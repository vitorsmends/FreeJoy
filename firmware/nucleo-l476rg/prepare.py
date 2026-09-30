#!/usr/bin/env python3
"""Stage our board and example in a pinned TinyUSB checkout (no source patching)."""
import argparse
from pathlib import Path
import shutil
import subprocess

HERE = Path(__file__).resolve().parent
REVISION = "86ad6e56c1700e85f1c5678607a762cfe3aa2f47"  # TinyUSB 0.18.0
parser = argparse.ArgumentParser()
parser.add_argument("--deps", action="store_true")
parser.add_argument("--tinyusb", type=Path, default=HERE / ".deps/tinyusb")
args = parser.parse_args()
root = args.tinyusb.resolve()
if args.deps and not root.exists():
    root.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(["git", "clone", "--depth", "1", "--branch", "0.18.0",
                    "https://github.com/hathach/tinyusb.git", str(root)], check=True)
if not (root / ".git").exists():
    raise SystemExit("Dependências ausentes. Execute make deps primeiro.")
revision = subprocess.check_output(["git", "-C", str(root), "rev-parse", "HEAD"], text=True).strip()
if revision != REVISION:
    raise SystemExit("Versão inesperada do TinyUSB; é necessário o commit " + REVISION)
if args.deps:
    subprocess.run(["python3", "tools/get_deps.py", "stm32l4"], cwd=root, check=True)
shutil.copytree(HERE / "board", root / "hw/bsp/stm32l4/boards/nucleo_l476rg_mock", dirs_exist_ok=True)
example = root / "examples/device/nucleo_mock_joystick"
shutil.copytree(HERE / "src", example / "src", dirs_exist_ok=True)
shutil.copyfile(HERE / "TinyUSB.mk", example / "Makefile")
for name in ("mock_inputs.h", "common_defines.h"):
    shutil.copyfile(HERE.parents[1] / "application/Inc" / name, example / "src" / name)
