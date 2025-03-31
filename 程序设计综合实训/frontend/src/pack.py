import os
import json

result = {}
for root, dirs, files in os.walk('.'):
    if 'node_modules' in dirs:
        dirs.remove('node_modules')
    for file in files:
        file_path = os.path.join(root, file)
        try:
            with open(file_path, 'r') as f:
                content = f.read()
                relative_path = os.path.relpath(file_path, '.')
                result[relative_path] = content
        except Exception as e:
            print(f"Error reading {file_path}: {e}")

with open('all_files.json', 'w') as f:
    json.dump(result, f)