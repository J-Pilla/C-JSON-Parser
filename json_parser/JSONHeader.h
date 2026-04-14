#pragma once

#include <inttypes.h>

#ifdef _MSC_VER
#ifdef JSON_EXPORTS
#define JSON_API __declspec(dllexport)
#else
#define JSON_API __declspec(dllimport)
#endif // JSON_EXPORTS
#else
#define JSON_API 
#endif // _MSC_VER

typedef enum JSONType
{
	JSON_TYPE_ERROR = -1,
	STRING,
	NUMBER,
	BOOLEAN,
	null // lower case to not conflict with NULL macro
} JSONType;

#ifdef _MSC_VER
int StringCopy(char* destination, uint64_t sizeInBytes, char const* source);
#else
char* StringCopy(char* destination, uint64_t size, char const* source);
#endif

int PrintError(const char* error);
