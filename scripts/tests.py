#!/usr/bin/env python3
import os
import subprocess

cc = "clang"
cflags: list[str] = [ "-std=c11", "-pedantic", "-Weverything", "-Og", "-ggdb", "-fsanitize=address,undefined" ]
cflags_str = " ".join(cflags)

paths: list[str] = []

for root, dirs, files in os.walk("tests/"):
	for f in files:
		if f.endswith(".c"):
			paths.append(os.path.join(root, f))

tests: list[str] = []

for f in paths:
	build_path = os.path.join("build", f.removesuffix(".c"))
	if not os.path.exists(os.path.dirname(build_path)):
		os.makedirs(os.path.dirname(build_path))

	command_str = f"{cc} -I./ {f} -o {build_path} {cflags_str}"
	print(command_str)

	subprocess.run(command_str, shell=True)
	tests.append(build_path)

for f in tests:
	command_str = os.path.join(".", f)
	print(command_str)
	subprocess.run(command_str, shell=True)
