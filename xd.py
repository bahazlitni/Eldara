import os

total = 0
for root, _, files in os.walk("."):
    if "eigen" in root.split(os.sep):
        continue
    for file in files:
        if file.endswith((".cpp", ".h")):
            try:
                with open(os.path.join(root, file), errors="ignore") as f:
                    total += sum(1 for _ in f)
            except Exception as e:
                print(f"Error reading {file}: {e}")
print(f"Total lines: {total}")
