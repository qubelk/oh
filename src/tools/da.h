// Macros for working with Dynamic Arrays
// Dynamic Array must be have fields named data, count and capacity
#ifndef OH_DA_H_
#define OH_DA_H_

#include <stdlib.h>

#define DA_APPEND(da, item) \
	do {\
		if (da.count >= da.capacity) {\
			if (da.capacity == 0) {\
				da.capacity = 4;\
			} else {\
				da.capacity *= 2;\
			}\
			da.data = realloc(da.data, da.capacity * sizeof(*da.data));\
		}\
		da.data[da.count++] = item;\
	} while (0)

// For now we need only DA_APPEND.

#endif
