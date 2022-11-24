#include "pch.h"
#include "publisher.h"

/*!
* Initialize List of publishers
*
* @param pubs		- Head of the list
*/
void InitPubs(Publisher** pubs)
{
	*pubs = NULL;
}

/*!
* Deinitialize list of publishers
*
* @param pubs		- Head of the list
*/
void DestroyPubs(Publisher** pubs)
{
	Publisher* current = *pubs;
	Publisher* next;

	while (current != NULL)
	{
		next = current->next;
		// Close and ShutDown socket
		shutdown(current->socket, SD_BOTH);
		closesocket(current->socket);
		current->socket = INVALID_SOCKET;
		free(current);
		current = next;
	}

	*pubs = NULL;
}

/*!
* Allocate memory for one publisher
*
* @return		- Pointer to created publisher
*/
Publisher* MakePub()
{
	Publisher* new_node = (Publisher*)malloc(sizeof(Publisher));
	if (new_node == NULL)
	{
		printf("\nNot enough RAM!\n");
		exit(13);
	}

	new_node->next = NULL;
	return new_node;
}

/*!
* Allocate new publisher and add it on the end of the list
*
* @param head		- Head of the list
*/
void AddOnEnd(Publisher** head)
{
	Publisher* node = MakePub();
	if (*head == NULL)
	{
		*head = node;
	}
	else
	{
		Publisher* temp = *head;
		// Itterate to node where next is null (last element)
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = node;
		node->next = NULL;
	}
}

/*!
* Add publiser on begin of the list
*
* @param head		- Head of the list
* @param node		- Publiser to add
*/
void AddOnBegin(Publisher** head, Publisher* node)
{
	if (*head == NULL)
	{
		*head = node;
	}
	else
	{
		Publisher* temp = *head;
		node->next = *head;
		*head = node;
	}
}

/*!
* Removes publisher from specific index
*
* @param head		- Head of the list
* @param index		- Value of Index
*/
void RemoveAt(Publisher** head, int index)
{
	if (index == 0)
	{
		// If element is head
		Publisher* toDelete = *head;
		Publisher* temp = (*head)->next;
		*head = temp;
		free(toDelete);
	}
	else
	{
		// If element is not head
		Publisher* temp = *head;
		for (int i = 0; i < (index - 1); ++i)
		{
			temp = temp->next;
			if (temp == NULL)
			{
				printf("\nIndex out of range!\n");
				return;
			}
		}

		Publisher* toDelete = temp->next;
		temp->next = toDelete->next;
		free(toDelete);
	}
}

/*!
* Returns number of elements in a list
*
* @param head		- Head of the List
*
* @return			- Number of elements
*/
int Count(Publisher* head)
{
	int count = 0;
	Publisher* temp = head;
	while (temp != NULL)
	{
		temp = temp->next;
		++count;
	}

	return count;
}