#!/usr/bin/env python3
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import time
import unittest


RUNNER = Path(__file__).with_name("vc_model_stress.sh").resolve()


class StressHarnessTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        self.binary = self.root / "model 'binary"
        self.binary.write_text("""#!/usr/bin/env python3
import json, os, pathlib, sys
seed, directory = sys.argv[1:3]
p = pathlib.Path(directory)
p.mkdir(exist_ok=True)
(p / 'invocation.json').write_text(json.dumps({
    'args': sys.argv[1:],
    'env': {k: v for k, v in os.environ.items() if k.startswith('VCMODEL_')},
}))
if os.environ.get('TICK_FILE'):
    pathlib.Path(os.environ['TICK_FILE']).touch()
if seed in ('13', '14'):
    if seed == '13': print('VIOLATION: injected failure')
    sys.exit(1)
""")
        self.binary.chmod(0o755)
        self.env = {k: v for k, v in os.environ.items() if not k.startswith("VCMODEL_")}
        self.env.update(VCMODEL_SECONDS="30", VCMODEL_PAR="2", VCMODEL_SEED="0")
        self.out = self.root / 'runs "$(touch injected)'

    def run_model(self, expected=0, **knobs):
        self.env.update(VCMODEL_OUT=str(self.out))
        before = set(self.out.glob("run.*"))
        result = subprocess.run(
            ["bash", str(RUNNER), str(self.binary)],
            env={**self.env, **knobs}, cwd=self.root,
            text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=15,
        )
        self.assertEqual(result.returncode, expected, result.stdout)
        if expected != 2:
            created = set(self.out.glob("run.*")) - before
            self.assertEqual(len(created), 1, result.stdout)
            self.run_out = created.pop()
        return result.stdout

    def test_clean_runs_report_both_layouts_and_shapes(self):
        self.run_model(VCMODEL_SEEDS="0 12")
        summary = (self.run_out / "summary.txt").read_text()
        self.assertIn("ok 0 attempt=1 processes=2 connections=1 busy=2000 clustered=0", summary)
        self.assertIn("ok 12 attempt=2 processes=2 connections=1 busy=300 clustered=1", summary)
        self.assertEqual(sorted(p.name for p in self.run_out.iterdir()), ["run-info.txt", "summary.txt"])

    def test_failure_preserves_each_duplicate_attempt(self):
        output = self.run_model(1, VCMODEL_SEEDS="13 13 12")
        self.assertEqual(output.count("VIOLATION: injected failure"), 2)
        self.assertEqual(len(list(self.out.rglob("s13-r*/invocation.json"))), 2)
        self.assertEqual(len(list(self.out.rglob("s13-r*.rerun.sh"))), 2)
        self.assertIn("1 seeds clean", output)

    def test_replay_preserves_environment_and_quotes_paths(self):
        self.run_model(1, VCMODEL_SEEDS="13", VCMODEL_PRELOG="1", VCMODEL_DESTRUCT="0", VCMODEL_CLUSTERED="0")
        original_path = next(self.out.rglob("s13-r1.*/invocation.json"))
        replay = Path(str(original_path.parent) + ".rerun.sh")
        result = subprocess.run(
            ["bash", str(replay)], cwd=self.root,
            env={**self.env, "VCMODEL_FOCUS": "main"}, timeout=15,
            stdout=subprocess.DEVNULL,
        )
        self.assertEqual(result.returncode, 1)
        original = json.loads(original_path.read_text())
        repeated = json.loads(Path(str(original_path.parent) + "-replay/invocation.json").read_text())
        self.assertEqual(original["env"], repeated["env"])
        self.assertEqual(original["args"][2:], repeated["args"][2:])
        self.assertFalse((self.root / "injected").exists())
        alternate = self.root / "alternate dir"
        result = subprocess.run(
            ["bash", str(replay), str(alternate)], cwd=self.root, env=self.env,
            timeout=15, stdout=subprocess.DEVNULL,
        )
        self.assertEqual(result.returncode, 1)
        self.assertTrue((alternate / "invocation.json").exists())

    def test_silent_failure_still_fails(self):
        output = self.run_model(1, VCMODEL_SEEDS="14")
        self.assertIn("--- seed 14", output)
        self.assertEqual(len(list(self.out.rglob("s14-r1.*.rerun.sh"))), 1)

    def test_reusing_output_directory_preserves_failures(self):
        self.run_model(1, VCMODEL_SEEDS="13")
        self.run_model(1, VCMODEL_SEEDS="13")
        self.assertEqual(len(list(self.out.rglob("s13-r1.*/invocation.json"))), 2)

    def test_concurrent_success_cannot_hide_failure(self):
        ready = self.root / "failure-ready"
        release = self.root / "release-failure"
        tools = self.root / "tools"
        tools.mkdir()
        grep = tools / "grep"
        grep.write_text('''#!/usr/bin/env bash
if [ "$1" = -c ] && [ "$2" = '^ok ' ]; then
  touch "$FAILURE_READY"
  while [ ! -f "$RELEASE_FAILURE" ]; do sleep 0.01; done
fi
exec "$REAL_GREP" "$@"
''')
        grep.chmod(0o755)
        failing = subprocess.Popen(
            ["bash", str(RUNNER), str(self.binary)], cwd=self.root,
            env={**self.env, "VCMODEL_OUT": str(self.out), "VCMODEL_SEEDS": "13",
                 "PATH": str(tools) + os.pathsep + self.env["PATH"],
                 "FAILURE_READY": str(ready), "RELEASE_FAILURE": str(release),
                 "REAL_GREP": shutil.which("grep")},
            text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        )
        try:
            deadline = time.monotonic() + 10
            while not ready.exists():
                self.assertIsNone(failing.poll())
                self.assertLess(time.monotonic(), deadline)
                time.sleep(0.01)
            self.run_model(VCMODEL_SEEDS="12")
            release.touch()
            output, _ = failing.communicate(timeout=10)
            self.assertEqual(failing.returncode, 1, output)
            self.assertIn("VIOLATION: injected failure", output)
            self.assertEqual(len(list(self.out.rglob("s13-r1.*/invocation.json"))), 1)
            self.assertEqual(len(list(self.out.rglob("s13-r1.*.rerun.sh"))), 1)
            summaries = list(self.out.glob("run.*/summary.txt"))
            self.assertEqual(len(summaries), 2)
            self.assertEqual(sum(p.read_text().startswith("FAIL ") for p in summaries), 1)
            self.assertEqual(len(list(self.out.glob("run.*/run-info.txt"))), 2)
        finally:
            release.touch()
            if failing.poll() is None:
                failing.kill()
            failing.communicate()

    def test_seed_limit_in_consecutive_mode(self):
        self.run_model(VCMODEL_SEED="2147483647")
        self.assertEqual(len((self.run_out / "summary.txt").read_text().splitlines()), 1)

    def test_invalid_settings_cannot_report_success(self):
        for knob, values in {
            "VCMODEL_SEEDS": ("", " ", "-1", "13 nope", "2147483648", "999999999999999999999"),
            "VCMODEL_SEED": ("oops", "-1"),
            "VCMODEL_CLUSTERED": ("", "2", "yes"),
            "VCMODEL_PAR": ("", "0"),
            "VCMODEL_SECONDS": ("", "0"),
        }.items():
            for value in values:
                with self.subTest(knob=knob, value=value):
                    self.assertIn("ERROR:", self.run_model(2, **{knob: value}))

    def test_incomplete_seed_list_fails(self):
        tick = self.root / "tick"
        date = self.root / "date"
        date.write_text('#!/usr/bin/env bash\nif [ -f "$TICK_FILE" ]; then echo 2; else echo 0; fi\n')
        date.chmod(0o755)
        self.env.update(PATH=str(self.root) + os.pathsep + self.env["PATH"], TICK_FILE=str(tick))
        output = self.run_model(1, VCMODEL_SEEDS="12 24", VCMODEL_PAR="1", VCMODEL_SECONDS="1")
        self.assertIn("time budget expired", output)
        self.assertEqual(len((self.run_out / "summary.txt").read_text().splitlines()), 1)


if __name__ == "__main__":
    unittest.main()
