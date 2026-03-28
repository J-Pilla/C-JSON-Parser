#include "header.h"

#include <string.h>

#ifdef _MSC_VER
int StringCopy(char* destination, rsize_t size, char const* source)
{
	return strcpy_s(destination, size, source);
}
#else
int StringCopy(char* destination, rsize_t sizeInBytes, char const* source)
{
	return strncpy(destination, source, size);
}
#endif
