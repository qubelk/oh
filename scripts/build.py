#!/usr/bin/env python3
import os
import subprocess

paths: list[str] = []

for root, dirs, files in os.walk("src/"):
	for f in files:
		if f.endswith(".c"):
			paths.append(os.path.join(root, f))

objects: list[str] = []

for f in paths:
	build_path = os.path.join("build", f)
	if not os.path.exists(os.path.dirname(build_path)):
		os.makedirs(os.path.dirname(build_path))

	# Use clang, for -Weverything flag
	subprocess.run(f"clang -Isrc/ -c {f} -o {build_path + ".o"} -Weverything", shell=True)
	objects.append(build_path + ".o")

build_path = os.path.join("build", "oh")
objects_string = " ".join(objects)
subprocess.run(f"clang -Isrc/ -o {build_path} {objects_string}", shell=True)
