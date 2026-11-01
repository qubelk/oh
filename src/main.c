#include "parser/parser.h"
#include "tools/die.h"
#include "exec/exec.h"

int main(int argc, char** argv) {
	if (!parser_init()) {
		die("Initialization failed. Aborting.\n");
	}

	Nodes nodes = parser_parse();
	exec_execute_ast(argc, argv, &nodes);

	parser_free(nodes);
	return 0;
}
