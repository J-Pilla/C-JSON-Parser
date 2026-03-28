#pragma once

#include <inttypes.h>

#ifdef JSON_EXPORTS
#define JSON_API __declspec(dllexport)
#else
#define JSON_API __declspec(dllimport)
#endif

int StringCopy(char* destination, rsize_t sizeInBytes, char const* source);
