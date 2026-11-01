#ifndef OH_EXEC_H_
#define OH_EXEC_H_

#include "ast/ast.h"

void exec_execute_rule(const char* name, const Nodes* nodes);
void exec_execute_ast(int argc, char** argv, const Nodes* nodes);

#endif
