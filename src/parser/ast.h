#ifndef OH_AST_H_
#define OH_AST_H_

#include "rules.h"

typedef enum {
	// Now supports only rules.
	NK_RULE,
} NodeKind;

typedef struct {
	NodeKind kind;
	union {
		Rule* rule;
	} as;
} Node;

Node* node_alloc(NodeKind kind);

typedef struct {
	Node** data;
	size_t count;
	size_t capacity;
} Nodes;

#endif
