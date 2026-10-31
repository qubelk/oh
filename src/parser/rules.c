#include "rules.h"

#include <stdlib.h>

Rule rule_new(char* name, FilePaths* deps, Commands* cmd) {
	return (Rule){
		.name = name,
		.deps = deps,
		.cmd = cmd,
	};
}
