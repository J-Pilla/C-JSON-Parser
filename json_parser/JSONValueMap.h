#pragma once

#include "JSONHeader.h"

typedef struct JSONValueMap
{
	struct JVMNode* first;
} JSONValueMap;


#define EMPTY_STRING_MAP (JSONValueMap){ (struct SMNode*)0 }

inline JSONValueMap JVMNewMap() { return EMPTY_STRING_MAP; }
int JVMFreeMap(JSONValueMap* map);
JSON_API const char* JVMGetValue(JSONValueMap* map, const char* key);
JSON_API JSONType JVMGetType(JSONValueMap* map, const char* key);
int JVMAdd(JSONValueMap* map, const char* value, JSONType type, const char* key);
