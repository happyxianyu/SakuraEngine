from pyuu.path import Path
import subprocess

self_path = Path(__file__)
root_path = self_path.prnt.prnt

# 读取排除文件列表
with open(root_path/'.mergeignore', 'r') as f:
    excluded_files = [line.strip() for line in f]

# 合并分支，并排除指定文件
def merge_with_exclusion(source_branch, target_branch):
    for excluded_file in excluded_files:
        subprocess.call(['git', 'checkout', target_branch, '--', excluded_file])

    subprocess.call(['git', 'merge', source_branch])

if __name__ == '__main__':
    source_branch = 'xy'  # 源分支
    target_branch = 'main'  # 目标分支

    merge_with_exclusion(source_branch, target_branch)
