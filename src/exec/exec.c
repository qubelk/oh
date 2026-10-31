#include "exec.h"

#include <stdio.h>
#include <stdlib.h>

void exec_execute_ast(const Nodes* nodes) {
	for (size_t i = 0; i < nodes->count; ++i) {
		Rule v = nodes->data[i].as.rule;
		for (size_t j = 0; j < v.commands->count; ++j) {
			printf("%s\n", v.commands->data[j]);
			system(v.commands->data[j]);
		}
	}
}
