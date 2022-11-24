#pragma once
#include "data.h"
#include "queue.h"
#include "constants.h"
#define SAFE_CLOSE(a) if(a){CloseHandle(a);}

/* Structre of Subscrber
   qeueue		- Queue for TopicChange-s
   socket		- Socket of given subscriber
   handle		- Handle for subscriber thread
   semaphore	- Semaphore for subscriber thread
   Id			- Id of subscriber thread
   next			- Pointer to the next subscriber in list
*/
typedef struct sub_st
{
	Queue queue;
	SOCKET socket;
	sub_st* next;
}Subscriber;

/*!
* Initialize List of subscribers
*
* @param subs		- Head of the list
*/
void InitSubs(Subscriber** subs);

/*!
* Deinitialize list of subscribers
*
* @param subs		- Head of the list
*/
void DestroySubs(Subscriber** subs);

/*!
* Allocate memory for one subscriber
*
* @return		- Pointer to created subscriber
*/
Subscriber* MakeSub();

/*!
* Add subscriber on the end of the list
*
* @param head		- Head of the list
* @param node		- Subscriber to add
*/
void AddOnEnd(Subscriber** head, Subscriber* node);

/*!
* Add subscriber on the begin of the list
*
* @param head		- Head of the list
* @param node		- Subscriber to add
*/
void AddOnBegin(Subscriber** head, Subscriber* node);

/*!
* Removes subscriber from specific index
*
* @param head		- Head of the list
* @param index		- Value of Index
*/
void RemoveAt(Subscriber** head, int index);

/*!
* Returns number of elements in a list
*
* @param head		- Head of the List
*
* @return			- Number of elements
*/
int Count(Subscriber* head);