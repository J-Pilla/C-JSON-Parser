#include "StringMap.h"

#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define SUCCESS 0
#define FAILURE 1

#define ERROR_PARAM_NULL "Error: NULL pointer passed"
#define ERROR_KEY_NOT_FOUND "Error: key not found in this map"
#define ERROR_KEY_EXISTS "Error: key already exists in this map"
#define ERROR_MAP_EMPTY "Error: this is empty"

typedef struct SMNode
{
	char* key;
	char* value;
	struct SMNode* next;
} SMNode;

static int error(const char*);
inline static void pause();

int SMFreeMap(StringMap* this)
{
	if (this == NULL)
		return error(ERROR_PARAM_NULL);

	SMNode* node = this->first;

	while (node != NULL)
	{
		this->first = this->first->next;
		free(node->key);
		free(node->value);
		free(node);
		node = this->first;
	}

	return SUCCESS;
}

const char* SMGetString(StringMap* this, const char* key)
{
	if (this == NULL || key == NULL)
	{
		error(ERROR_PARAM_NULL);
		return NULL;
	}

	SMNode* node = this->first;

	while (node != NULL)
	{
		if (strcmp(node->key, key) == 0)
			return node->value;

		node = node->next;
	}

	error(ERROR_KEY_NOT_FOUND);
	return NULL;
}

int SMSetString(StringMap* this, const char* value, const char* key)
{
	if (this == NULL || value == NULL || key == NULL)
		return error(ERROR_PARAM_NULL);

	SMNode* node = this->first;

	while (node != NULL)
	{
		if (strcmp(node->key, key) == 0)
			break;

		node = node->next;
	}

	if (node == NULL)
		return error(ERROR_KEY_NOT_FOUND);

	size_t length = strlen(value) + 1ull;

	free(node->value);

	node->value = malloc(length);
	assert(node->value);

	strcpy_s(node->value, length, value);

	return SUCCESS;
}

int SMAdd(StringMap* this, const char* value, const char* key)
{
	if (this == NULL || value == NULL || key == NULL)
		return error(ERROR_PARAM_NULL);

	SMNode* builder = malloc(sizeof(SMNode));
	assert(builder);

	size_t keySize = strlen(key) + 1ull;
	size_t valueSize = strlen(value) + 1ull;


	*builder = (SMNode)
	{
		malloc(keySize),
		malloc(valueSize),
		NULL
	};
	assert(builder->key);
	assert(builder->value);

	strcpy_s(builder->key, keySize, key);
	strcpy_s(builder->value, valueSize, value);

	if (this->first != NULL)
	{
		SMNode* node = this->first;
		
		while (node->next != NULL)
		{
			if (strcmp(node->key, key) == 0)
			{
				free(builder->key);
				free(builder->value);
				free(builder);
				return error(ERROR_KEY_EXISTS);
			}

			node = node->next;
		}

		node->next = builder;
	}
	else
		this->first = builder;

	return SUCCESS;
}

int SMRemove(StringMap* this, const char* key)
{
	if (this == NULL || key == NULL)
		return error(ERROR_PARAM_NULL);

	if (this->first == NULL)
		return error(ERROR_MAP_EMPTY);

	SMNode* currentNode = this->first, * prevNode = NULL;

	while (currentNode->next != NULL)
	{
		if (strcmp(currentNode->key, key) == 0)
			break;

		prevNode = currentNode;
		currentNode = currentNode->next;
	}

	if (currentNode == NULL)
		return error(ERROR_KEY_NOT_FOUND);

	if (currentNode == this->first)
		this->first = currentNode->next;
	else
		prevNode->next = currentNode->next;

	free(currentNode->key);
	free(currentNode->value);
	free(currentNode);

	return SUCCESS;
}

static int error(const char* error)
{
	if (error == NULL)
		error = "Error";

	puts(error);
	pause();
	return FAILURE;
}

inline static void pause()
{
	printf("press enter to continue . . . ");
	int key = getchar();
	fflush(stdin);
}
