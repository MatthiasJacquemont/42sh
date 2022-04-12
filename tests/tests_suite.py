from argparse import ArgumentParser
from pathlib import Path
from difflib import unified_diff
from dataclasses import dataclass, field

from termcolor import colored

import subprocess as sp
import yaml


OK_TAG = f"[{colored('OK', 'green')}]"
KO_TAG = f"[{colored('KO', 'red')}]"

@dataclass
class TestCase:
    name: str
    input: str
    checks: list= field(default_factory=lambda: ["stdout", "exitcode"]) #stderr

print("")

def diff(expected: str, actual: str) -> str:
    expected_lines = expected.splitlines(keepends=True)
    actual_lines = actual.splitlines(keepends=True)
    return ''.join(unified_diff(expected_lines, actual_lines, fromfile='Matthias expected', tofile='actual'))

def run_shell(shell: str, stdin: str) -> sp.CompletedProcess:
    return sp.run([shell], input=stdin, capture_output=True, text=True)

def perform_check(expected: sp.CompletedProcess, actual: sp.CompletedProcess, checks):
    if ("has_stderr" in checks and actual.stderr == ""):
        print("Something was expected on stderr")
        return 1

    if (expected.returncode != actual.returncode):
        print(f"Exited with {actual.returncode} expected {expected.returncode}")

    if ("stdout" in checks and expected.stdout != actual.stdout):
        print(f"stdout differ\n{diff(expected.stdout, actual.stdout)}")
        return 1

    elif ("stderr" in checks and expected.stderr != actual.stderr):
        print(f"stderr differ\n{diff(expected.stderr, actual.stderr)}")
        return 1
    print(actual.stdout)
    print(expected.stdout)
    return 0


if __name__ == "__main__":
    parser = ArgumentParser("Testsuite")
    parser.add_argument("--binary", required=True, type=Path)
    args = parser.parse_args()

    binary_path = args.binary.absolute() #path of the binary

    with open("tests.yml", "r") as file:
        testsuite = [TestCase(**testcase) for testcase in  yaml.safe_load(file)]

    for testcase in testsuite:
        stdin = testcase.input
        name = testcase.name

        dash_proc = run_shell("dash", stdin)
        sh_proc = run_shell(binary_path, stdin)

#        print(dash_proc.stdout)

        res = perform_check(dash_proc, sh_proc, testcase.checks)
        if res == 1:
            print(f"{KO_TAG} {name}")
            print("_________________\n")
        else:
            print(f"{OK_TAG} {name}\n")

    print()
