#include "parser/parser.h"
#include "tools/die.h"

int main(int argc, char** argv) {
	if (!parser_init()) {
		die("Initialization failed. Aborting.\n");
	}

	parser_free();
	return 0;
}
