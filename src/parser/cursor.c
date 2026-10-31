#include "cursor.h"

char cursor_peek(Cursor* cursor) {
	return cursor->line[cursor->pos];
}

void cursor_advance(Cursor* cursor) {
	cursor->pos++;
}
