#pragma once

#include "header.h"

typedef struct StringList
{
	struct SLNode* first;
	int length;
} StringList;


#define EMPTY_STRING_LIST (StringList){ (struct SLNode *)0, 0 }

JSON_API inline StringList SLNewList() { return EMPTY_STRING_LIST; }
JSON_API int SLFreeList(StringList* this);
JSON_API const char* SLGetString(StringList* this, int index);
JSON_API int SLSetString(StringList* this, const char* value, int index);
JSON_API int SLPush(StringList* this, const char* value);
JSON_API int SLPop(StringList* this);
