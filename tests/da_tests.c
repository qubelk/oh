#include "src/tools/da.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
	int* data;
	size_t count;
	size_t capacity;
} Integers;

static void da_integers_test(void) {
	Integers nums = {0};
	// Append 5 items, so capacity must be 8.
	for (size_t i = 0; i < 5; ++i) {
		DA_APPEND(nums, i + 1);
	}

	assert(nums.count == 5);
	assert(nums.capacity == 8);

	for (size_t i = 0; i < nums.count; ++i) {
		assert(nums.data[i] == i + 1);
	}

	DA_FREE(nums);
	puts("da_tests: da_integers_test passed.");
}

typedef struct {
	char** data;
	size_t count;
	size_t capacity;
} Strings;

static void da_strings_test(void) {
	Strings strs = {0};
	// Append 3 items, so capacity must be standart 4.
	for (size_t i = 0; i < 3; ++i) {
		char* item = "Hello, World!";

		DA_APPEND(strs, item);
	}

	assert(strs.count == 3);
	assert(strs.capacity == 4);

	for (size_t i = 0; i < strs.count; ++i) {
		assert(strcmp(strs.data[i], "Hello, World!") == 0);
	}

	DA_FREE(strs);
	puts("da_tests: da_strings_test passed.");
}

typedef struct {
	int** data;
	size_t count;
	size_t capacity;
} Pointers;

static void da_pointers_test(void) {
	Pointers p = {0};

	for (size_t i = 0; i < 5; ++i) {
		int* ptr = malloc(sizeof(*ptr));
		*ptr = i + 1;
		DA_APPEND(p, ptr);
	}

	assert(p.count == 5);
	assert(p.capacity == 8);

	for (size_t i = 0; i < p.count; ++i) {
		assert(*(p.data[i]) == i + 1);
	}

	DA_POINTERS_FREE(p);
	puts("da_tests: da_pointers_test passed.");
}

int main(void) {
	da_integers_test();
	da_strings_test();
	da_pointers_test();
	puts("da_tests: All tests passed.");
	return 0;
}
