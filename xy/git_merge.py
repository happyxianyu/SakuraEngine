merge_ignores = """
.vscode
xy
tmp
.gitignore

""".strip().split('\n')



import subprocess
import os

from pyuu.path import Path

run_cmd = os.system

self_path = Path(__file__)
root_path = self_path.prnt.prnt

os.chdir(root_path)
run_cmd('git checkout xy-main')
run_cmd('git merge xy')
run_cmd('git reset main')

for ignore in merge_ignores:
    run_cmd(f'git reset -- {Path(ignore).quote}')


