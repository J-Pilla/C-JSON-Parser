#pragma once

#include "JSONHeader.h"

#include "JSONValueMap.h"
#include "JSONValueList.h"

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
	JSONValueMap values;
	void* parent;
} JSONObject;

typedef struct JSONArray
{
	JSONList objects;
	JSONList arrays;
	JSONValueList values;
	void* parent;
} JSONArray;

JSON_API JSONList JSONParse(const char* file);
JSON_API int JSONFree(JSONList* list);

JSON_API union JSON* JSONListGet(JSONList* list, int index);
JSON_API union JSON* JSONMapGet(JSONMap* map, const char* key);
