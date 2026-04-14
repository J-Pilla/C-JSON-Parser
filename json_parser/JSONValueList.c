#include "JSONValueList.h"

#include <assert.h>
#include <malloc.h>
#include <string.h>

#define SUCCESS 0
#define FAILURE 1

#define ERROR_PARAM_NULL "NULL pointer passed"
#define ERROR_OUT_OF_BOUNDS "out of bounds of the list"
#define ERROR_LIST_EMPTY "list is empty"

typedef struct JVLNode
{
	char* value;
	JSONType type;
	struct JVLNode* next;
} JVLNode;

int JVLFreeList(JSONValueList* list)
{
	if (list == NULL)
		return PrintError(ERROR_PARAM_NULL);

	JVLNode* node = list->first;

	while (node != NULL)
	{
		list->first = node->next;
		free(node->value);
		free(node);
		node = list->first;
	}

	list->length = 0;

	return SUCCESS;
}

const char* JVLGetValue(JSONValueList* list, int index)
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

	JVLNode* node = list->first;

	for (int ctr = 0; ctr < index; ctr++)
		node = node->next;

	return node->value;
}

JSONType JVLGetType(JSONValueList* list, int index)
{
	if (list == NULL)
	{
		PrintError(ERROR_PARAM_NULL);
		return JSON_TYPE_ERROR;
	}

	if (index < 0 || index >= list->length)
	{
		PrintError(ERROR_OUT_OF_BOUNDS);
		return JSON_TYPE_ERROR;
	}

	JVLNode* node = list->first;

	for (int ctr = 0; ctr < index; ctr++)
		node = node->next;

	return node->type;
}

int JVLPush(JSONValueList* list, const char* value, JSONType type)
{
	if (list == NULL || value == NULL)
		return PrintError(ERROR_PARAM_NULL);

	JVLNode* builder = malloc(sizeof(JVLNode));
	assert(builder);

	size_t size = strlen(value) + 1ull;

	*builder = (JVLNode)
	{
		malloc(size),
		type,
		NULL
	};
	assert(builder->value);

	StringCopy(builder->value, size, value);

	if (list->first != NULL)
	{
		JVLNode* node = list->first;

		while (node->next != NULL)
			node = node->next;

		node->next = builder;
	}
	else
		list->first = builder;

	list->length++;

	return SUCCESS;
}
