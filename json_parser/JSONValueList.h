#pragma once

#include "JSONHeader.h"

typedef struct JSONValueList
{
	struct JVLNode* first;
	int length;
} JSONValueList;


#define EMPTY_STRING_LIST (JSONValueList){ (struct SLNode *)0, 0 }

inline JSONValueList JVLNewList() { return EMPTY_STRING_LIST; }
int JVLFreeList(JSONValueList* list);
JSON_API const char* JVLGetValue(JSONValueList* list, int index);
JSON_API JSONType JVLGetType(JSONValueList* list, int index);
int JVLPush(JSONValueList* list, const char* value, JSONType type);
