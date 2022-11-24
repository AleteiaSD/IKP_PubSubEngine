#pragma once
#include "customCommunication.h"
#define SAFE_CLOSE(a) if(a){CloseHandle(a);}

/* Structre of Publisher
   socket - Socket of given publisher
   handle - Handle for publisher thread
   Id	  - Id of publisher thread
   next	  - Pointer to the next publisher in list
*/
typedef struct pub_st
{
	SOCKET socket;
	pub_st* next;
}Publisher;

/*!
* Initialize List of publishers
*
* @param pubs		- Head of the list
*/
void InitPubs(Publisher** pubs);

/*!
* Deinitialize list of publishers
*
* @param pubs		- Head of the list
*/
void DestroyPubs(Publisher** pubs);

/*!
* Allocate memory for one publisher
*
* @return		- Pointer to created publisher
*/
Publisher* MakePub();

/*!
* Allocate new publisher and add it on the end of the list
*
* @param head		- Head of the list
*/
void AddOnEnd(Publisher** head);

/*!
* Add publiser on begin of the list
*
* @param head		- Head of the list
* @param node		- Publiser to add
*/
void AddOnBegin(Publisher** head, Publisher* node);

/*!
* Removes publisher from specific index
*
* @param head		- Head of the list
* @param index		- Value of Index
*/
void RemoveAt(Publisher** head, int index);

/*!
* Returns number of elements in a list
*
* @param head		- Head of the List
*
* @return			- Number of elements
*/
int Count(Publisher* head);