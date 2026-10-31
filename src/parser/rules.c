#include "rules.h"

#include <stdlib.h>

Rule rule_new(char* name, FilePaths* deps, Commands* commands) {
	return (Rule){
		.name = name,
		.deps = deps,
		.commands = commands,
	};
}
