#pragma once

typedef struct StringList
{
	struct SLNode* first;
	int length;
} StringList;


#define EMPTY_STRING_LIST (StringList){ (struct SLNode *)0, 0 }

inline StringList SLNewList() { return EMPTY_STRING_LIST; }
int SLFreeList(StringList* this);
const char* SLGetString(StringList* this, int index);
int SLSetString(StringList* this, const char* value, int index);
int SLPush(StringList* this, const char* value);
int SLPop(StringList* this);
