#ifndef OH_CURSOR_H_
#define OH_CURSOR_H_

#include <stddef.h>

typedef struct {
	struct {
		char* data;
		size_t count;
		size_t capacity;
	} input;

	// Current position.
	size_t pos;
} Cursor;

char cursor_peek(const Cursor* cursor);
void cursor_advance(Cursor* cursor);

#endif
