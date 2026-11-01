#include "exec.h"

#include "tools/die.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static Commands* search_rule_name(const char* name, const Nodes* nodes) {
	for (size_t i = 0; i < nodes->count; ++i) {
		Rule v = nodes->data[i].as.rule;
		if (strcmp(name, v.name) == 0) {
			return v.commands;
		}
	}

	return NULL;
}

void exec_execute_rule(const char* name, const Nodes* nodes) {
	Commands* commands = search_rule_name(name, nodes);
	if (commands == NULL) {
		die("Unknown rule %s, call valid rule.\n", name);
	}

	for (size_t i = 0; i < commands->count; ++i) {
		printf("%s\n", commands->data[i]);
		system(commands->data[i]);
	}
}

void exec_execute_ast(int argc, char** argv, const Nodes* nodes) {
	if (argc >= 3) {
		fprintf(stderr, "Support only one rule name\n");
		die("Use: ./oh <rule>, not ./oh <rule> <rule> ...");
	}

	for (size_t i = 1; i < argc; ++i) {
		exec_execute_rule(argv[i], nodes);
	}
}
