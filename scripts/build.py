#!/usr/bin/env python3
import os
import subprocess


# Use clang, for -Weverything flag
cc = "clang"
cflags: list[str] = [ "-std=c11", "-pedantic", "-Weverything", "-Og", "-ggdb", "-fsanitize=address,undefined" ]
cflags_str = " ".join(cflags)

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

	command_str = f"{cc} -Isrc/ -c {f} -o {build_path + ".o"} {cflags_str}"
	print(command_str)

	subprocess.run(command_str, shell=True)
	objects.append(build_path + ".o")

build_path = os.path.join("build", "oh")
objects_str = " ".join(objects)

command_str = f"{cc} -Isrc/ -o {build_path} {objects_str} {cflags_str}"
print(command_str)
subprocess.run(command_str, shell=True)
