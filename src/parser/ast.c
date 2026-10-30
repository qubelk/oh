#include "ast.h"

#include <stdlib.h>

Node* node_alloc(NodeKind kind) {
	Node* node = malloc(sizeof(*node));
	if (node == NULL) {
		perror("Allocation failed");
		return NULL;
	}

	node->kind = kind;
	return node;
}
