#pragma once

#include "header.h"

#include "StringMap.h"
#include "StringList.h"

typedef struct JSONList
{
	struct JSONListNode* first;
	int length;
} JSONList;

typedef struct JSONMap
{
	struct JSONMapNode* first;
} JSONMap;

typedef struct JSONObject
{
	JSONMap objects;
	JSONMap arrays;
	StringMap values;
	void* parent;
} JSONObject;

typedef struct JSONArray
{
	JSONList objects;
	JSONList arrays;
	StringList values;
	void* parent;
} JSONArray;

JSON_API JSONList JSONParse(const char* file);
JSON_API int JSONFree(JSONList* this);

JSON_API union JSON* JSONListGet(JSONList* this, int index);
JSON_API union JSON* JSONMapGet(JSONMap* this, const char* key);
