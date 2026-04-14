#pragma once

#include "JSONHeader.h"

typedef struct JSONValueMap
{
	struct JVMNode* first;
} JSONValueMap;


#define EMPTY_VALUE_MAP (JSONValueMap){ (struct JVMNode*)0 }

inline JSONValueMap JVMNewMap() { return EMPTY_VALUE_MAP; }
int JVMFreeMap(JSONValueMap* map);
JSON_API const char* JVMGetValue(JSONValueMap* map, const char* key);
JSON_API JSONType JVMGetType(JSONValueMap* map, const char* key);
int JVMAdd(JSONValueMap* map, const char* value, JSONType type, const char* key);
