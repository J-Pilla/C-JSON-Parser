#include "JSONHeader.h"

#include <stdio.h>
#include <string.h>

#define FAILURE 1

#ifdef _MSC_VER
int StringCopy(char* destination, uint64_t size, char const* source) { return strcpy_s(destination, size, source); }
#else
char* StringCopy(char* destination, uint64_t size, char const* source) { return strncpy(destination, source, size); }
#endif

static inline int pause()
{
	printf("press enter to continue . . . ");
	fflush(stdin);
	return getchar();
}

int PrintError(const char* error)
{
	if (error == NULL)
		error = "";

	printf("Error: %s\n", error);
	pause();
	return FAILURE;
}
