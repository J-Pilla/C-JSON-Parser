#pragma once

typedef struct StringList
{
	struct SLNode* firstNode;
	int length;
} StringList;


#define EMPTY_STRING_LIST (StringList){ ((void *)0), 0 }

inline StringList SLConstruct() { return EMPTY_STRING_LIST; }
int SLFreeList(StringList* list);
const char* SLGetString(StringList* list, int index);
int SLSetString(StringList* list, char* value, int index);
int SLPush(StringList* list, char* value);
int SLPop(StringList* list);
