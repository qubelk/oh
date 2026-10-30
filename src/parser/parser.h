#ifndef OH_PARSER_H_
#define OH_PARSER_H_

#include "ast.h"

// Search and open `ohfile` or `Ohfile` in root directory which `oh` cli called.
// @return
// 	0: if init failed.
// 	1: if init success.
int parser_init(void);

// Clean all parser internal data.
void parser_free(void);

Nodes parser_parse(void);

#endif
