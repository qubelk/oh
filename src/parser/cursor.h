#ifndef OH_CURSOR_H_
#define OH_CURSOR_H_

#include <stddef.h>

typedef struct {
	// Current line.
	char* line;

	// Current position.
	size_t pos;
} Cursor;

char cursor_peek(Cursor* cursor);
void cursor_advance(Cursor* cursor);

#endif
