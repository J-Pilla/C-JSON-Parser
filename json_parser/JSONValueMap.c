#include "JSONValueMap.h"

#include <assert.h>
#include <malloc.h>
#include <string.h>

#define SUCCESS 0
#define FAILURE 1

#define ERROR_PARAM_NULL "NULL pointer passed"
#define ERROR_KEY_NOT_FOUND "key not found in map"
#define ERROR_KEY_EXISTS "key already exists in map"
#define ERROR_MAP_EMPTY "map is empty"

typedef struct JVMNode
{
	char* key;
	char* value;
	JSONType type;
	struct JVMNode* next;
} JVMNode;

int JVMFreeMap(JSONValueMap* map)
{
	if (map == NULL)
		return PrintError(ERROR_PARAM_NULL);

	JVMNode* node = map->first;

	while (node != NULL)
	{
		map->first = map->first->next;
		free(node->key);
		free(node->value);
		free(node);
		node = map->first;
	}

	return SUCCESS;
}

const char* JVMGetValue(JSONValueMap* map, const char* key)
{
	if (map == NULL || key == NULL)
	{
		PrintError(ERROR_PARAM_NULL);
		return NULL;
	}

	JVMNode* node = map->first;

	while (node != NULL)
	{
		if (strcmp(node->key, key) == 0)
			return node->value;

		node = node->next;
	}

	PrintError(ERROR_KEY_NOT_FOUND);
	return NULL;
}

JSONType JVMGetType(JSONValueMap* map, const char* key)
{
	if (map == NULL || key == NULL)
	{
		PrintError(ERROR_PARAM_NULL);
		return JSON_TYPE_ERROR;
	}

	JVMNode* node = map->first;

	while (node != NULL)
	{
		if (strcmp(node->key, key) == 0)
			return node->type;

		node = node->next;
	}

	PrintError(ERROR_KEY_NOT_FOUND);
	return JSON_TYPE_ERROR;
}

int JVMAdd(JSONValueMap* map, const char* value, JSONType type, const char* key)
{
	if (map == NULL || value == NULL || key == NULL)
		return PrintError(ERROR_PARAM_NULL);

	JVMNode* builder = malloc(sizeof(JVMNode));
	assert(builder);

	size_t keySize = strlen(key) + 1ull;
	size_t valueSize = strlen(value) + 1ull;


	*builder = (JVMNode)
	{
		malloc(keySize),
		malloc(valueSize),
		type,
		NULL
	};
	assert(builder->key);
	assert(builder->value);

	StringCopy(builder->key, keySize, key);
	StringCopy(builder->value, valueSize, value);

	if (map->first != NULL)
	{
		JVMNode* node = map->first;
		
		while (node->next != NULL)
		{
			if (strcmp(node->key, key) == 0)
			{
				free(builder->key);
				free(builder->value);
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
