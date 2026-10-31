#include "ast.h"

#include "tools/da.h"
#include <stdlib.h>

Node node_new(NodeKind kind) {
	return (Node){
		.kind = kind,
	};
}
