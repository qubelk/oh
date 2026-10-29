#include "die.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void die(const char* s, ...) {
	va_list params;

	va_start(params, s);
	vfprintf(stderr, s, params);
	va_end(params);

	exit(EXIT_FAILURE);
}
