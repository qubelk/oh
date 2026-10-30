#ifndef OH_FILES_H_
#define OH_FILES_H_

#include <stdio.h>

// Dynamic array of Files.
typedef struct {
	FILE** data;
	size_t count;
	size_t capacity;
} Files;

#endif
