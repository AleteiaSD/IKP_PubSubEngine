#include "pch.h"
#include "topichangeList.h"

/*!
* Initialize List of Nodes
*
* @param head		- Head of the list
*/
void init_list(NODE** head)
{
	*head = NULL;
}

/*!
* Deinitialize List of Nodes
*
* @param head		- Head of the list
*/
void destroy_list(NODE** head)
{
	NODE* current = *head;
	NODE* next;

	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}

	*head = NULL;
}

/*!
* Prints List of Nodes
*
* @param head		- Head of the list
*/
void print_list(NODE* head)
{
	printf("List:\n");
	NODE* temp = head;
	while (temp != NULL)
	{
		printf("%s %s %lf", GetStringFromSignalType(temp->value.topic.signal), GetStringFromDeviceType(temp->value.topic.device), temp->value.value);
		temp = temp->next;
	}
	printf("\n\n");
}

/*!
* Allocate memory for one node
*
* @param		- TopicChange value
*
* @return 		- Pointer to created Node
*/
NODE* make_node(TopicChange value)
{
	NODE* new_node = (NODE*)malloc(sizeof(NODE));
	if (new_node == NULL)
	{
		printf("\nNot enough RAM!\n");
		exit(13);
	}

	new_node->value.value = value.value;
	new_node->value.topic.device = value.topic.device;
	new_node->value.topic.signal = value.topic.signal;

	new_node->next = NULL;
	return new_node;
}

/*!
* Allocates new node and node on the begin of the list
*
* @param head		- Head of the list
* @param value		- Value of TopicChage
*/
void add_on_begin(NODE** head, TopicChange value)
{
	NODE* node = make_node(value);
	if (*head == NULL)
	{
		*head = node;
	}
	else
	{
		NODE* temp = *head;
		node->next = *head;
		*head = node;
	}
}


/*!
* Allocates new node and node on the end of the list
*
* @param head		- Head of the list
* @param value		- Value of TopicChage
*/
void add_on_end(NODE** head, TopicChange value)
{
	NODE* node = make_node(value);
	if (*head == NULL)
	{
		*head = node;
	}
	else
	{
		NODE* temp = *head;
		// Itterate to node where next is null (last element)
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = node;
		node->next = NULL;
	}
}

/*!
* Returns element at the specific index, if exist
*
* @param head		- Head of the list
* @param index		- Value of index
*
* @return			- Pointer to specific TopicChange
*/
TopicChange* elementAt(NODE* head, int index)
{
	NODE* temp = head;
	for (int i = 0; i < index; ++i)
	{
		temp = temp->next;
		if (temp == NULL)
		{
			printf("\nIndex out of range!\n");
			return NULL;
		}
	}

	return &(temp->value);
}

/*!
* Returns element at the specific index, if exist
*
* @param head		- Head of the list
* @param index		- Value of index
* @param value		- Value of TopicChange
*/
void insert(NODE** head, int index, TopicChange value)
{
	NODE* node = make_node(value);
	if (index == 0)
	{
		// If inserting on head
		NODE* temp = *head;
		node->next = *head;
		*head = node;
	}
	else
	{
		// If inserting to other position
		NODE* temp = *head;
		for (int i = 0; i < (index - 1); ++i)
		{
			temp = temp->next;
			if (temp == NULL)
			{
				printf("\nIndex out of range!\n");
				return;
			}
		}

		NODE* nxt = temp->next;
		temp->next = node;
		node->next = nxt;
	}
}

/*!
* Removes element at the specific index
*
* @param head		- Head of the list
* @param index		- Value of index
*/
void removeAt(NODE** head, int index)
{
	if (index == 0)
	{
		// If element is head
		NODE* toDelete = *head;
		NODE* temp = (*head)->next;
		*head = temp;
		if (toDelete != NULL)
		{
			free(toDelete);
			toDelete = NULL;
		}

	}
	else
	{
		// If element is not head
		NODE* temp = *head;
		for (int i = 0; i < (index - 1); ++i)
		{
			temp = temp->next;
			if (temp == NULL)
			{
				printf("\nIndex out of range!\n");
				return;
			}
		}

		NODE* toDelete = temp->next;
		temp->next = toDelete->next;
		if (toDelete != NULL)
		{
			free(toDelete);
			toDelete = NULL;
		}
	}
}

/*!
* Returns number of elements in a list
*
* @param head		- Head of the list
*
* @return			- Number of elements
*/
int count(NODE* head)
{
	int count = 0;
	NODE* temp = head;
	while (temp != NULL)
	{
		temp = temp->next;
		++count;
	}

	return count;
}