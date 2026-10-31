#ifndef OH_COMMANDS_H_
#define OH_COMMANDS_H_

#include "files.h"

typedef struct {
	char** data;
	size_t count;
	size_t capacity;
} Commands;

typedef struct {
	// Name of command should be end with `:` prefix.
	// Example: `build:`, `run:`, etc.
	char* name;

	// Array of dependency, oh check modify time of this for caching.
	// Dependency are optional, in this case is be NULL.
	// Dependecy syntax are, ( file1 | file2 | ... ) after, command name.
	// Example: `build: ( main.c | foo.h | foo.c )`
	FilePaths* deps;

	// Cmd is a body of command, be called by shell.
	Commands* commands;
} Rule;

Rule rule_new(char* name, FilePaths* deps, Commands* commands);

#endif
