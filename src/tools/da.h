// Macros for working with Dynamic Arrays
// Dynamic Array must be have fields named data, count and capacity
#ifndef OH_DA_H_
#define OH_DA_H_

#include <stdlib.h>

#define DA_APPEND(da, item) \
	do {\
		if ((da).count >= (da).capacity) {\
			if ((da).capacity == 0) {\
				(da).capacity = 4;\
			} else {\
				(da).capacity *= 2;\
			}\
			(da).data = realloc((da).data, (da).capacity * sizeof(*(da).data));\
		}\
		(da).data[(da).count++] = item;\
	} while (0)

#define DA_FREE(da) \
	do {\
		free((da).data);\
	} while (0)

#define DA_POINTERS_FREE(da) \
	do {\
		for (size_t i = 0; i < (da).count; ++i) {\
			free((da).data[i]);\
		}\
		free((da).data);\
	} while (0)

#define DAP_POINTERS_FREE(da) \
	do {\
		for (size_t i = 0; i < (da)->count; ++i) {\
			free((da)->data[i]);\
		}\
		free((da)->data);\
		free((da));\
	} while (0)

#endif
