#pragma once
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include "conio.h"
#include "topichangeList.h"

/* Thread-safe implementation of a queue structure based on one-way linekd list
 * head				- Head of the list
 * cs				- Critical Section for thread safe implementation
 *
 * InitQueue		- Initialize List and Critical Section
 * DestroyQueue		- Deinitialize List and Critical Section
 * Put				- Puts value in a queue
 * Get				- Gets value from a queue
 * Count			- Return number of elements in a queue
 *
*/
typedef struct queue_st
{
	NODE* head;
	CRITICAL_SECTION cs;

	// Queue initialization
	void InitQueue()
	{
		init_list(&head);
		//Spin count is 10 - 10 times tries to enter before wait on semaphore
		InitializeCriticalSectionEx(&cs, 10, CRITICAL_SECTION_NO_DEBUG_INFO);
	}

	// Destruction of Queue
	void DestroyQueue()
	{
		destroy_list(&head);
		DeleteCriticalSection(&cs);
	}

	// Put element in Queue
	void Put(TopicChange value)
	{
		EnterCriticalSection(&cs);
		add_on_end(&head, value);
		LeaveCriticalSection(&cs);
	}

	// Get element from Queue
	TopicChange Get()
	{
		EnterCriticalSection(&cs);
		TopicChange item = *(elementAt(head, 0));
		removeAt(&head, 0);
		LeaveCriticalSection(&cs);
		return item;
	}

	// Get count of element in Queue
	int Count()
	{
		EnterCriticalSection(&cs);
		int result = count(head);
		LeaveCriticalSection(&cs);
		return result;
	}
}Queue;