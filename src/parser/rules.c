#include "rules.h"

#include <stdlib.h>

Rule* rule_alloc(char* name, FilePaths* deps, char* cmd) {
	Rule* rule = malloc(sizeof(*rule));
	if (rule == NULL) {
		perror("Allocation failed");
		return NULL;
	}

	rule->name = name;
	rule->deps = deps;
	rule->name = name;

	return rule;
}
