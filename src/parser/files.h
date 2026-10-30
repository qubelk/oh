#ifndef OH_FILES_H_
#define OH_FILES_H_

#include <stdio.h>

// Dynamic array of File paths.
typedef struct {
	char** data;
	size_t count;
	size_t capacity;
} FilePaths;

#endif
