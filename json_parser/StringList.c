#include "StringList.h"

#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define SUCCESS 0
#define FAILURE 1

#define ERROR_PARAM_NULL "Error: NULL pointer passed"
#define ERROR_OUT_OF_BOUNDS "Error: out of bounds of the list"
#define ERROR_LIST_EMPTY "Error: this is empty"

typedef struct SLNode
{
	char* value;
	struct SLNode* next;
} SLNode;

static int error(const char*);
inline static void pause();

int SLFreeList(StringList* this)
{
	if (this == NULL)
		return error(ERROR_PARAM_NULL);

	SLNode* node = this->first;

	while (node != NULL)
	{
		this->first = this->first->next;
		free(node->value);
		free(node);
		node = this->first;
	}

	this->length = 0;

	return SUCCESS;
}

const char* SLGetString(StringList* this, int index)
{
	if (this == NULL)
	{
		error(ERROR_PARAM_NULL);
		return NULL;
	}

	if (index < 0 || index >= this->length)
	{
		error(ERROR_OUT_OF_BOUNDS);
		return NULL;
	}

	SLNode* node = this->first;

	for (int ctr = 0; ctr < index; ctr++)
	{
		node = node->next;
	}

	return node->value;
}

int SLSetString(StringList* this, const char* value, int index)
{
	if (this == NULL || value == NULL)
		return error(ERROR_PARAM_NULL);

	if (index < 0 || index >= this->length)
		return error(ERROR_OUT_OF_BOUNDS);

	SLNode* node = this->first;
	size_t size = strlen(value) + 1ull;

	for (int ctr = 0; ctr < index; ctr++)
	{
		node = node->next;
	}

	free(node->value);

	node->value = malloc(size);
	assert(node->value);

	StringCopy(node->value, size, value);

	return SUCCESS;
}

int SLPush(StringList* this, const char* value)
{
	if (this == NULL || value == NULL)
		return error(ERROR_PARAM_NULL);

	SLNode* builder = malloc(sizeof(SLNode));
	assert(builder);

	size_t size = strlen(value) + 1ull;

	*builder = (SLNode)
	{
		malloc(size),
		NULL
	};
	assert(builder->value);

	StringCopy(builder->value, size, value);

	if (this->first != NULL)
	{
		SLNode* node = this->first;

		while (node->next != NULL)
		{
			node = node->next;
		}

		node->next = builder;
	}
	else
		this->first = builder;

	this->length++;

	return SUCCESS;
}

int SLPop(StringList* this)
{
	if (this == NULL)
		return error(ERROR_PARAM_NULL);

	if (this->first == NULL)
		return error(ERROR_LIST_EMPTY);

	SLNode* currentNode = this->first, * prevNode = NULL;

	while (currentNode->next != NULL)
	{
		prevNode = currentNode;
		currentNode = currentNode->next;
	}

	if (currentNode == this->first)
		this->first = currentNode->next;
	else
		prevNode->next = currentNode->next;

	free(currentNode->value);
	free(currentNode);
	this->length--;

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
