#pragma once

#include "header.h"

typedef struct StringMap
{
	struct SMNode* first;
} StringMap;


#define EMPTY_STRING_MAP (StringMap){ (struct SMNode*)0 }

JSON_API inline StringMap SMNewMap() { return EMPTY_STRING_MAP; }
JSON_API int SMFreeMap(StringMap* this);
JSON_API const char* SMGetString(StringMap* this, const char* key);
JSON_API int SMSetString(StringMap* this, const char* value, const char* key);
JSON_API int SMAdd(StringMap* this, const char* value, const char* key);
JSON_API int SMRemove(StringMap* this, const char* key);
