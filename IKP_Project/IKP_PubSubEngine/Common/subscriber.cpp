#include "pch.h"
#include "subscriber.h"

/*!
* Initialize List of subscribers
*
* @param subs		- Head of the list
*/
void InitSubs(Subscriber** subs)
{
	*subs = NULL;
}

/*!
* Deinitialize list of subscribers
*
* @param subs		- Head of the list
*/
void DestroySubs(Subscriber** subs)
{
	Subscriber* current = *subs;
	Subscriber* next;

	while (current != NULL)
	{
		next = current->next;
		// Destroies queue
		current->queue.DestroyQueue();
		shutdown(current->socket, SD_BOTH);
		closesocket(current->socket);
		current->socket = INVALID_SOCKET;
		free(current);
		current = next;
	}

	*subs = NULL;
}

/*!
* Allocate memory for one subscriber
*
* @return		- Pointer to created subscriber
*/
Subscriber* MakeSub()
{
	Subscriber* new_node = (Subscriber*)malloc(sizeof(Subscriber));
	if (new_node == NULL)
	{
		printf("\nNot enough RAM!\n");
		exit(13);
	}

	new_node->queue.InitQueue();

	new_node->next = NULL;
	return new_node;
}

/*!
* Add subscriber on the end of the list
*
* @param head		- Head of the list
* @param node		- Subscriber to add
*/
void AddOnEnd(Subscriber** head, Subscriber* node)
{
	if (*head == NULL)
	{
		*head = node;
	}
	else
	{
		Subscriber* temp = *head;
		// Itterate to node where next is null (last element)
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = node;
		node->next = NULL;
	}
}

/*!
* Add subscriber on the begin of the list
*
* @param head		- Head of the list
* @param node		- Subscriber to add
*/
void AddOnBegin(Subscriber** head, Subscriber* node)
{
	if (*head == NULL)
	{
		*head = node;
	}
	else
	{
		Subscriber* temp = *head;
		node->next = *head;
		*head = node;
	}
}

/*!
* Removes subscriber from specific index
*
* @param head		- Head of the list
* @param index		- Value of Index
*/
void RemoveAt(Subscriber** head, int index)
{
	if (index == 0)
	{
		// If element is head
		Subscriber* toDelete = *head;
		Subscriber* temp = (*head)->next;
		*head = temp;
		toDelete->queue.DestroyQueue();
		free(toDelete);
	}
	else
	{
		// If element is not head
		Subscriber* temp = *head;
		for (int i = 0; i < (index - 1); ++i)
		{
			temp = temp->next;
			if (temp == NULL)
			{
				printf("\nIndex out of range!\n");
				return;
			}
		}

		Subscriber* toDelete = temp->next;
		temp->next = toDelete->next;
		toDelete->queue.DestroyQueue();
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
int Count(Subscriber* head)
{
	int count = 0;
	Subscriber* temp = head;
	while (temp != NULL)
	{
		temp = temp->next;
		++count;
	}

	return count;
}