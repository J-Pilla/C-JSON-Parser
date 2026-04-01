#include "header.h"

#include <string.h>

#ifdef _MSC_VER
int StringCopy(char* destination, uint64_t size, char const* source)
{
	return strcpy_s(destination, size, source);
}
#else
char* StringCopy(char* destination, uint64_t size, char const* source)
{
	return strncpy(destination, source, size);
}
#endif
