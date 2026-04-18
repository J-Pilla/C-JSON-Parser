#include "JSON.h"

#include <assert.h>
#include <ctype.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SUCCESS 0
#define FAILURE 1

#define ERROR_PARAM_NULL "NULL pointer passed"
#define ERROR_FILE_OPEN "unable to open file"
#define ERROR_FILE_FORMAT "file cannot be parsed"
#define ERROR_FILE_CLOSE "unable to close file"
#define ERROR_FILE_EMPTY "file empty"
#define ERROR_OUT_OF_BOUNDS "out of bounds of the list"
#define ERROR_KEY_NOT_FOUND "key not found in map"
#define ERROR_KEY_EXISTS "key already exists in map"
#define ERROR_ARRAY_EMPTY "array is empty"

typedef struct JSONList List;
typedef struct JSONMap Map;
typedef struct JSONObject Object;
typedef struct JSONArray Array;

union JSON
{
	JSONObject object;
	JSONArray array;
};

typedef bool CollectionType;
#define ARRAY 0
#define OBJECT 1

typedef struct JSONCollection
{
	union JSON json;
	CollectionType type;
} JSON;

typedef struct JSONListNode
{
	JSON value;
	struct ListNode* next;
} ListNode;

typedef struct JSONMapNode
{
	char* key;
	JSON value;
	struct MapNode* next;
} MapNode;

#define EMPTY_LIST (List){ NULL, 0 }
#define EMPTY_MAP (Map){ NULL }

#define EMPTY_OBJECT (Object)	\
{								\
	EMPTY_MAP,					\
	EMPTY_MAP,					\
	EMPTY_VALUE_MAP,			\
	NULL						\
}

#define EMPTY_ARRAY (Array)	\
{							\
	EMPTY_LIST,				\
	EMPTY_LIST,				\
	EMPTY_VALUE_LIST,		\
	NULL					\
}

#define EMPTY_JSON (union JSON) { EMPTY_OBJECT }

#define blankJson(type) (JSON)	\
{								\
	EMPTY_JSON,					\
	type						\
}

static char* fileToString(const char* const);

// list functions
static JSON* allocateJsonL(union JSON*, CollectionType);
static JSON* getJsonL(List*, int);
static int pushJsonL(List*, JSON);

// map functions
static JSON* allocateJsonM(union JSON*, CollectionType, const char*);
static JSON* getJsonM(Map*, const char*);
static int addJsonM(Map*, JSON, const char*);

// key / value functions
static char* allocateString(const char*, size_t*);
static char* allocateNumber(const char*, size_t*);

// type functions
static int pushType(CollectionType**, CollectionType, int*);
static int popType(CollectionType**, int*);

// memory cleanup
inline static int freeJson(JSON*);
static int freeObject(Object*);
static int freeArray(Array*);
static int freeList(List*);
static int freeMap(Map*);

JSONList JSONParse(const char* file)
{
	char* json = fileToString(file);

	if (json == NULL || sizeof(json) < 5)
	{
		PrintError(ERROR_FILE_FORMAT);
		return EMPTY_LIST;
	}

	size_t jsonLength = strlen(json);

	// typeStack tracks the hiarchy of the JS objects
	CollectionType* typeStack = NULL;
	int stackDepth = 0;

	// cursor walks along json
	size_t cursor = 0;

	for (; cursor < jsonLength - 1; cursor++)
	{
		// check if the file starts with an array or object
		if (json[cursor] == '[')
		{
			pushType(&typeStack, ARRAY, &stackDepth);

			if (stackDepth == 2)
				break;
		}
		else
		{
			if (json[cursor] == '{')
			{
				pushType(&typeStack, OBJECT, &stackDepth);
				cursor++;
			}
			break;
		}
	}

	if (typeStack == NULL || typeStack[stackDepth - 1] != OBJECT)
	{
		free(json);
		free(typeStack);
		PrintError(ERROR_FILE_FORMAT);
		return EMPTY_LIST;
	}

	List list = EMPTY_LIST;

	// a json to build the current node
	JSON builder = blankJson(OBJECT);

	// a pointer to work on the current depth
	JSON* currentJson = &builder;

	// ROOT tracks the base level for objects
	const int ROOT = stackDepth - 1;

	for (; cursor < jsonLength - 1; cursor++)
	{
		if (currentJson->type == OBJECT)
		{
			char* key = NULL;

			if (json[cursor] == '\"') // signs a key
			{
				key = allocateString(json, &cursor);

				if (key == NULL)
				{
					free(json);
					free(typeStack);
					freeList(&list);
					return EMPTY_LIST;
				}

				cursor += 2;

				switch (json[cursor])
				{
				case '{':
				case '[':
				{
					CollectionType type = json[cursor] == '{' ? OBJECT : ARRAY;
					currentJson = allocateJsonM(&currentJson->json, type, key);

					if (currentJson == NULL)
					{
						free(json);
						free(typeStack);
						freeList(&list);
						free(key);
						return EMPTY_LIST;
					}

					pushType(
						&typeStack,
						type,
						&stackDepth
					);
					break;
				} 
				case '\"':
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case 't':
				case 'f':
				case 'n':
					switch (json[cursor])
					{
					case '\"':
						JVMAdd(
							&currentJson->json.object.values,
							allocateString(json, &cursor),
							STRING,
							key
						);
						break;
					case 't':
						JVMAdd(
							&currentJson->json.object.values,
							"true",
							BOOLEAN,
							key
						);
						cursor += 3;
						break;
					case 'f':
						JVMAdd(
							&currentJson->json.object.values,
							"false",
							BOOLEAN,
							key
						);
						cursor += 4;
						break;
					case 'n':
						JVMAdd(
							&currentJson->json.object.values,
							"null",
							null,
							key
						);
						cursor += 3;
						break;
					default:
						JVMAdd(
							&currentJson->json.object.values,
							allocateNumber(json, &cursor),
							NUMBER,
							key
						);
					}

					if (JVMGetValue(&currentJson->json.object.values, key) == NULL)
					{
						free(json);
						free(typeStack);
						freeList(&list);
						free(key);
						return EMPTY_LIST;
					}
				}

				free(key);
				key = NULL;
			}
			else if (json[cursor] == '}')
			{
				if (currentJson->json.object.parent != NULL)
					currentJson = currentJson->json.object.parent;

				popType(&typeStack, &stackDepth);
			}
		}
		else // type == array
		{
			switch (json[cursor])
			{
			case '{':
			case '[':
			{
				CollectionType type = json[cursor] == '{' ? OBJECT : ARRAY;
				currentJson = allocateJsonL(&currentJson->json, type);

				if (currentJson == NULL)
				{
					free(json);
					free(typeStack);
					freeList(&list);
					return EMPTY_LIST;
				}

				pushType(
					&typeStack,
					type,
					&stackDepth
				);
				break;
			}
			case '\"':
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 't':
			case 'f':
			case 'n':
				switch (json[cursor])
				{
				case '\"':
					JVLPush(
						&currentJson->json.array.values,
						allocateString(json, &cursor),
						STRING
					);
					break;
				case 't':
					JVLPush(
						&currentJson->json.array.values,
						"true",
						BOOLEAN
					);
					cursor += 3;
					break;
				case 'f':
					JVLPush(
						&currentJson->json.array.values,
						"false",
						BOOLEAN
					);
					cursor += 4;
					break;
				case 'n':
					JVLPush(
						&currentJson->json.array.values,
						"null",
						null
					);
					cursor += 3;
					break;
				default:
					JVLPush(
						&currentJson->json.array.values,
						allocateNumber(json, &cursor),
						NUMBER
					);
				}

				if (JVLGetValue(
					&currentJson->json.array.values,
					currentJson->json.array.values.length - 1
				) == NULL)
				{
					free(json);
					free(typeStack);
					freeList(&list);
					return EMPTY_LIST;
				}
				break;
			case ']':
				currentJson = currentJson->json.array.parent;
				popType(&typeStack, &stackDepth);
			}
		}

		if (stackDepth == ROOT && json[cursor] == ',')
		{
			pushJsonL(&list, builder);

			builder = blankJson(OBJECT);
			currentJson = &builder;

			pushType(&typeStack, OBJECT, &stackDepth);
			cursor++;
		}
	}

	pushJsonL(&list, builder);
	free(json);
	free(typeStack);
	return list;
}

int JSONFree(JSONList* list) { return freeList(list); }

union JSON* JSONListGet(JSONList* list, int index) { return &getJsonL(list, index)->json; }

union JSON* JSONMapGet(JSONMap* map, const char* key) { return &getJsonM(map, key)->json; }

static char* fileToString(const char* const file)
{
	FILE* fin = fopen(file, "r");

	if (fin == NULL)
	{
		printf("%s | ", file);
		PrintError(ERROR_FILE_OPEN);
		return NULL;
	}

	char** buffer = calloc(1, sizeof(char*));
	assert(buffer);

	const int BUFFER = 80;
	int bufferCount = 0;

	size_t cursor = 0, characterCount = 0;

	// while loop header
	{
		int previousInput = 0;
		bool isInString = false;
		while (!feof(fin))
		{
			if (cursor == BUFFER - 1)
			{
				buffer[bufferCount][cursor] = '\0';
				bufferCount++;
				cursor = 0;
				char** temp = realloc(buffer, sizeof(char*) * (bufferCount + 1));
				assert(temp);
				buffer = temp;
			}

			if (cursor == 0)
			{
				buffer[bufferCount] = malloc(BUFFER);
				assert(buffer[bufferCount]);
			}

			int input = fgetc(fin);

			if ((char)input == '"' && (char)previousInput != '\\')
				isInString = !isInString;

			if (isspace(input) != 0 && !isInString)
				continue;

			previousInput = input;
			buffer[bufferCount][cursor] = input >= 0 ? (char)input : '\0';
			characterCount++;
			cursor++;
		}
	}

	if (characterCount < 2)
	{
		free(buffer[0]);
		free(buffer);

		printf("%s | ", file);
		PrintError(ERROR_FILE_FORMAT);
		return NULL;
	}

	char* string = calloc(characterCount, sizeof(char));
	assert(string);

	cursor = 0;

	for (int stringIndex = 0, bufferIndex = 0;
		stringIndex < characterCount;
		stringIndex++, cursor++)
	{
		if (cursor == BUFFER - 1)
		{
			bufferIndex++;
			cursor = 0;
		}

		string[stringIndex] = buffer[bufferIndex][cursor];
	}

	for (int index = 0; index < bufferCount; index++)
		free(buffer[index]);

	free(buffer);

	if (fclose(fin) != 0)
	{
		printf("%s | ", file);
		PrintError(ERROR_FILE_CLOSE);
		return NULL;
	}

	return string;
}

// list functions
static JSON* allocateJsonL(union JSON* json, CollectionType type)
{
	if (json == NULL)
	{
		PrintError(ERROR_PARAM_NULL);
		return NULL;
	}

	List* list = type == OBJECT ?
		&json->array.objects : &json->array.arrays;

	pushJsonL(list, blankJson(type));

	JSON* child = getJsonL(list, list->length - 1);

	if (child->type == OBJECT)
		child->json.object.parent = json;
	else
		child->json.array.parent = json;

	return child;
}

static JSON* getJsonL(List* list, int index)
{
	if (list == NULL)
	{
		PrintError(ERROR_PARAM_NULL);
		return NULL;
	}

	if (index < 0 || index >= list->length)
	{
		PrintError(ERROR_OUT_OF_BOUNDS);
		return NULL;
	}

	ListNode* currentNode = list->first;

	for (int ctr = 0; ctr < index; ctr++)
		currentNode = currentNode->next;

	return &currentNode->value;
}

static int pushJsonL(List* list, JSON value)
{
	if (list == NULL)
		return PrintError(ERROR_PARAM_NULL);

	ListNode* builder = malloc(sizeof(ListNode));
	assert(builder);

	*builder = (ListNode)
	{
		value,
		NULL
	};

	if (list->first)
	{
		ListNode* node = list->first;

		while (node->next)
			node = node->next;

		node->next = builder;
	}
	else
		list->first = builder;

	list->length++;

	return SUCCESS;
}

// map functions
static JSON* allocateJsonM(union JSON* json, CollectionType type, const char* key)
{
	if (json == NULL || key == NULL)
	{
		PrintError(ERROR_PARAM_NULL);
		return NULL;
	}

	Map* map = type == OBJECT ?
		&json->object.objects : &json->object.arrays;

	addJsonM(map, blankJson(type), key);

	JSON* child = getJsonM(map, key);

	if (child->type == OBJECT)
		child->json.object.parent = json;
	else
		child->json.array.parent = json;

	return child;
}

static JSON* getJsonM(Map* map, const char* key)
{
	if (map == NULL || key == NULL)
	{
		PrintError(ERROR_PARAM_NULL);
		return NULL;
	}

	MapNode* node = map->first;

	while (node != NULL)
	{
		if (strcmp(node->key, key) == 0)
			return &node->value;

		node = node->next;
	}

	PrintError(ERROR_KEY_NOT_FOUND);
	return NULL;
}

static int addJsonM(Map* map, JSON value, const char* key)
{
	if (map == NULL || key == NULL)
		return PrintError(ERROR_PARAM_NULL);

	MapNode* builder = malloc(sizeof(MapNode));
	assert(builder);

	size_t size = strlen(key) + 1ull;

	*builder = (MapNode)
	{
		malloc(size),
		value,
		NULL
	};
	assert(builder->key);

	StringCopy(builder->key, size, key);

	if (map->first != NULL)
	{
		MapNode* node = map->first;

		while (node->next != NULL)
		{
			if (strcmp(node->key, key) == 0)
			{
				free(builder->key);
				free(builder);
				return PrintError(ERROR_KEY_EXISTS);
			}

			node = node->next;
		}

		node->next = builder;
	}
	else
		map->first = builder;

	return SUCCESS;
}

// key / value functions
static char* allocateString(const char* JSON, size_t* cursor)
{
	int size = 0;
	while (JSON[*cursor + ++size] != '\0')
	{
		if (JSON[*cursor + size] == '\"' && JSON[*cursor + size - 1] != '\\')
			break;
	}

	if (JSON[*cursor + size] == '\0')
	{
		PrintError(ERROR_FILE_FORMAT);
		return NULL;
	}

	char* string = malloc(size);
	assert(string);

	int index = 0;

	for (; index < size - 1; index++)
		string[index] = JSON[*cursor + index + 1];

	string[index] = '\0';

	*cursor += size;

	return string;
}

static char* allocateNumber(const char* JSON, size_t* cursor)
{
	int size = 0;
	(*cursor)--;
	while (JSON[*cursor + ++size] != '\0')
	{
		bool isBreaking = false;

		switch (JSON[*cursor + size])
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
			break;
		default:
			isBreaking = true;
		}

		if (isBreaking)
			break;
	}

	if (JSON[*cursor + size] == '\0')
	{
		PrintError(ERROR_FILE_FORMAT);
		return NULL;
	}

	char* string = malloc(size);
	assert(string);

	int index = 0;

	for (; index < size - 1; index++)
		string[index] = JSON[*cursor + index + 1];

	string[index] = '\0';

	*cursor += size - 1;

	return string;
}

// type functions
static int pushType(CollectionType** stack, CollectionType attribute, int* depth)
{
	if (stack == NULL || depth == NULL)
		return PrintError(ERROR_PARAM_NULL);

	(*depth)++;

	if (*stack == NULL)
	{
		*stack = calloc(*depth, sizeof(CollectionType));
		assert(*stack);
	}
	else
	{
		CollectionType* temp = realloc(*stack, sizeof(CollectionType) * (*depth));
		assert(temp);
		*stack = temp;
	}

	(*stack)[(*depth) - 1] = attribute;

	return SUCCESS;
}

static int popType(CollectionType** stack, int* depth)
{
	if (stack == NULL || depth == NULL)
		return PrintError(ERROR_PARAM_NULL);

	if (depth == 0)
		return PrintError(ERROR_ARRAY_EMPTY);

	(*depth)--;

	if (*depth > 0)
	{
		CollectionType* temp = realloc(*stack, sizeof(CollectionType) * (*depth));
		assert(temp);
		*stack = temp;
	}
	else
	{
		free(*stack);
		*stack = NULL;
	}

	return SUCCESS;
}

// memory cleanup
inline static int freeJson(JSON* json)
{
	return json->type == OBJECT ?
		freeObject(&json->json) : freeArray(&json->json);
}

static int freeObject(Object* obj)
{
	if (obj == NULL)
		return PrintError(ERROR_PARAM_NULL);

	freeMap(&obj->objects);
	freeMap(&obj->arrays);
	JVMFreeMap(&obj->values);
	obj->parent = NULL;

	return SUCCESS;
}

static int freeArray(Array* array)
{
	if (array == NULL)
		return PrintError(ERROR_PARAM_NULL);

	freeList(&array->objects);
	freeList(&array->arrays);
	JVLFreeList(&array->values);
	array->parent = NULL;

	return SUCCESS;
}

static int freeList(List* list)
{
	if (list == NULL)
		return PrintError(ERROR_PARAM_NULL);

	ListNode* node = list->first;

	while (node != NULL)
	{
		list->first = list->first->next;
		freeJson(&node->value);
		free(node);
		node = list->first;
	}

	list->length = 0;

	return SUCCESS;
}

static int freeMap(Map* map)
{
	if (map == NULL)
		return PrintError(ERROR_PARAM_NULL);

	MapNode* node = map->first;

	while (node != NULL)
	{
		map->first = map->first->next;
		free(node->key);
		freeJson(&node->value);
		free(node);
		node = map->first;
	}

	return SUCCESS;
}
