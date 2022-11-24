#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>
#include "conio.h"
#include "publisher.h"
#include "PubSubFunctions.h"

/* Global Variables */
int ch;
bool endFlag = true;
unsigned long int mode = 1;
int sockAddrLen = sizeof(struct sockaddr);

// Semaphore for processing semaphore, to sync Processing thread and Publisher thread
HANDLE processingPub;
// Semaphore for processing semaphore, to sync Processing thread and Subscriber thread
HANDLE processingSub;
// Sempahore for processing semaphore, to sync Subscriber thread and Processing thread
HANDLE subscribers;
// Semaphore for processing semaphore, to sync Publisher thread and Processing thread
HANDLE publishers;
// Handle for Processing thread
HANDLE processingThread;
// Handle for Listen Thread
HANDLE listenHandle;
// Handle for Publisher thread
HANDLE pubHandle;
// Handle for Subscriber thread
HANDLE subHandle;

// Ids
DWORD processingThreadId;
DWORD listenId;
DWORD pubHandleId;
DWORD subHandleId;

// Publisher and Subscriber listen sockets
SOCKET pubListenSocket;
SOCKET subListenSocket;

// List of Pubs
Publisher* pubs;
// Main Queue
Queue queue;
// List of Subscribers
Subscriber* allSubscribers;
// Dictionary<Topic, List<Subscriber>>
KeyValuePair dictionary[6];

#pragma region Thread Functions

/* Recieves data from Publisher */
void RecvDataFromPub(Publisher* pub, int i) 
{
	bool res;
	int len = 0;
	// Recieve Heder
	res = CustomRecv(pub->socket, (char*)&len, sizeof(int));
	char* buffer = NULL;
	if (res)
	{
		// Alocate memory based on length in header
		buffer = (char*)malloc(sizeof(char) * len);
		int count = len / sizeof(TopicChange);
		printf("[Pub] Recieved[%d] = %d\n", i, count);
		// Recieve all data
		if (CustomRecv(pub->socket, buffer, len))
		{
			TopicChange* temp = (TopicChange*)buffer;
			for (int i = 0; i < count; i++)
			{
				// Put change in main queue
				queue.Put(temp[i]);
				// if end?
				if (!endFlag)
					break;
			}
		}
		free(buffer);
		
	}
}

/* Thread Function - Handles publishers */ 
DWORD WINAPI PublisherThread(LPVOID param)
{
	int res;
	FD_SET read;
	timeval timeVal;
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 0;
	bool pubErr = false;
	while (endFlag)
	{
		int count = Count(pubs);
		if (count > 0)
		{
			Publisher* pub = pubs;
			for (int i = 0; i < count; i++)
			{
				if (pub == NULL)
					continue;
				FD_ZERO(&read);
				FD_SET(pub->socket, &read);
				pubErr = false;
				res = select(0, &read, NULL, NULL, &timeVal);
				if (res == SOCKET_ERROR)
				{
					printf("select failed with error %d\n", WSAGetLastError());
					free(pub);
					pub = NULL;
					break;
				}
				else if (res == 0)
				{
					Sleep(SLEEP_TIME);
					if (!endFlag)
						break;
				}
				else if (FD_ISSET(pub->socket, &read))
				{
					RecvDataFromPub(pub, i);
				}
				pub = pub->next;
				if (pub == NULL)
					continue;
			}
			ReleaseSemaphore(processingPub, 1, 0);
			WaitForSingleObject(publishers, INFINITE);
		}
	}
	return 0;
}

/* Sends data to Subscriber */
void SendDataToSub(Subscriber* sub)
{
	bool result;
	int count = sub->queue.Count();
	printf("[Sub] To send = %d\n", count);
	int len = count * sizeof(TopicChange);
	// Copy data from subscribers queue to local buffer
	char* buffer = (char*)malloc(len);
	TopicChange* topics = (TopicChange*)buffer;
	for (int i = 0; i < count; i++)
	{
		TopicChange tc = sub->queue.Get();
		topics[i] = tc;
	}
	// Send header, with length of data
	result = CustomSend(sub->socket, (char*)&len, sizeof(int));
	if (result)
	{
		// Send data
		result = CustomSend(sub->socket, buffer, len);
		if (!result)
			sub->socket = INVALID_SOCKET;
		free(buffer);
	}
	else
	{
		free(buffer);
	}
}

/* Thread Function - Handles subscribers */
DWORD WINAPI SubscriberThread(LPVOID param)
{
	Subscriber* sub = (Subscriber*)param;
	bool result;
	while (endFlag)
	{
		WaitForSingleObject(subscribers, INFINITE);
		for (int i = 0; i < 6; i++) 
		{
			Subscriber* item = dictionary[i].subs;
			int count = Count(item);
			if (count <= 0)
				continue;
			for (int j = 0; j < count; j++) 
			{
				if(item->queue.Count() > 0 && item->socket != INVALID_SOCKET)
					SendDataToSub(item);
				item = item->next;
				if (item == NULL)
					break;
			}
		}
		ReleaseSemaphore(processingSub, 1, 0);
	}
	return 0;
}

/* Thread Function - Handles processing of changes */ 
DWORD WINAPI ProcessingThread(LPVOID param)
{
	while (endFlag)
	{
		// Wait for processing semaphore to be released
		WaitForSingleObject(processingPub, INFINITE);
		if (!endFlag)
			break;
		int count = queue.Count();
		if (count <= 0)
			continue;
		printf("[Processing] Count = %d\n", count);
		for (int i = 0; i < count; i++)
		{
			// For every item in main queue 
			TopicChange item = queue.Get();
			for (int j = 0; j < 6; j++)
			{
				// Find topic of that item in dictionary
				if (dictionary[j].topic.Equals(item.topic))
				{
					int subCount = Count(dictionary[j].subs);
					// Get subscribers for that topic
					Subscriber* subb = dictionary[j].subs;
					for (int k = 0; k < subCount; k++)
					{
						// Put data in subscribers queue
						subb->queue.Put(item);
						subb = subb->next;
						if (!endFlag)
							break;
					}
				}
			}
		}
		ReleaseSemaphore(subscribers, 1, 0);
		ReleaseSemaphore(publishers, 1, 0);
		WaitForSingleObject(processingSub, INFINITE);
	}
	return 0;
}

/* Thread Function - Listens for publishers, creates them and creates thread for publisher */
DWORD WINAPI ListenThread(LPVOID param)
{
	bool result;
	int res;
	FD_SET read;
	timeval timeVal;
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 0;
	sockaddr_in clientAddress;

	while (endFlag)
	{
		FD_ZERO(&read);
		FD_SET(pubListenSocket, &read);
		FD_SET(subListenSocket, &read);

		res = select(0, &read, NULL, NULL, &timeVal);
		if (res == SOCKET_ERROR)
		{
			printf("select failed with error %d\n", WSAGetLastError());
			break;
		}
		else if (res == 0)
		{
			Sleep(SLEEP_TIME);
			if (!endFlag)
				break;
		}
		else if (FD_ISSET(pubListenSocket, &read))
		{
			// Make new publisher
			Publisher* newNode = MakePub();
			newNode->socket = accept(pubListenSocket, (struct sockaddr*)&clientAddress, &sockAddrLen);
			if (newNode->socket == INVALID_SOCKET)
			{
				if (WSAGetLastError() == WSAECONNRESET)
					printf("accept failed,timeout has expired!\n");
				else
					printf("accept failed with error %d\n", WSAGetLastError());
			}
			else
			{
				if (ioctlsocket(newNode->socket, FIONBIO, &mode) != 0) {
					printf("ioctl failed with error %d\n", WSAGetLastError());
					continue;
				}
				printf("New Publisher!\n");
				AddOnBegin(&pubs, newNode);
			}

		}
		else if (FD_ISSET(subListenSocket, &read))
		{
			// Make new subscriber
			Subscriber* newNode = MakeSub();
			newNode->socket = accept(subListenSocket, (struct sockaddr*)&clientAddress, &sockAddrLen);
			if (newNode->socket == INVALID_SOCKET)
			{
				if (WSAGetLastError() == WSAECONNRESET)
					printf("accept failed,timeout has expired!\n");
				else
					printf("accept failed with error %d\n", WSAGetLastError());
			}
			else
			{
				if (ioctlsocket(newNode->socket, FIONBIO, &mode) != 0) {
					printf("ioctl failed with error %d\n", WSAGetLastError());
					continue;
				}

				int len = 0;
				// Recieve header of subscribed topics - SUBSCRIPTION
				result = CustomRecv(newNode->socket, (char*)&len, sizeof(int));
				if (result)
				{
					int count = len / sizeof(Topic);
					char* buffer = (char*)malloc(len);
					// Recieve all topics for SUBSCRIPTION
					result = CustomRecv(newNode->socket, buffer, len);
					if (result)
					{
						Topic* topics = (Topic*)buffer;
						for (int i = 0; i < count; i++)
						{
							for (int j = 0; j < 6; j++)
							{
								// Add subscriber to topics in dictionary
								if (dictionary[j].topic.Equals(topics[i]))
								{
									AddOnBegin(&(dictionary[j].subs), newNode);
								}
							}
						}
					}
					free(buffer);
				}
				printf("New Subscriber!\n");
				AddOnBegin(&allSubscribers, newNode);
			}
		}
	}
	return 0;
}

#pragma endregion

/*  Global Functions */
void SafeExit();				//Deinitialze all used data
BOOL CTRLHandler(DWORD event);  // Handle Close event of a console

int main(void)
{
	// WinSock Lib Initialization
	if (!InitializeWindowsSockets())
		return 1;

	// Set Console Close event handler - Graceful shutdown
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CTRLHandler, TRUE);

	int iResult;
	pubListenSocket = INVALID_SOCKET;
	subListenSocket = INVALID_SOCKET;
	queue.InitQueue();
	InitSubs(&allSubscribers);
	GenerateDictionaryItems(dictionary);

#pragma region SocketInitialization

	sockaddr_in pubServerAddress;
	pubServerAddress.sin_family = AF_INET;
	pubServerAddress.sin_addr.s_addr = inet_addr(DEFAULT_IP);
	pubServerAddress.sin_port = htons(PUB_PORT);

	sockaddr_in subServerAddress;
	subServerAddress.sin_family = AF_INET;
	subServerAddress.sin_addr.s_addr = inet_addr(DEFAULT_IP);
	subServerAddress.sin_port = htons(SUB_PORT);

	// Initialize PubListen socket
	pubListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (pubListenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		SafeExit();
		return 1;
	}

	// Initialize SubListen socket
	subListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (subListenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		SafeExit();
		return 1;
	}

	// Bind

	iResult = bind(pubListenSocket, (sockaddr*)&pubServerAddress, sockAddrLen);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		SafeExit();
		return 1;
	}

	iResult = bind(subListenSocket, (sockaddr*)&subServerAddress, sockAddrLen);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		SafeExit();
		return 1;
	}

	// Set non-blocking mode

	if (ioctlsocket(pubListenSocket, FIONBIO, &mode) == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		SafeExit();
		return 1;
	}

	if (ioctlsocket(subListenSocket, FIONBIO, &mode) == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		SafeExit();
		return 1;
	}

	// Start listening

	iResult = listen(pubListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		SafeExit();
		return 1;
	}

	iResult = listen(subListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		SafeExit();
		return 1;
	}
#pragma endregion

	// Threads and Semaphores
	listenHandle = CreateThread(NULL, 0, &ListenThread, NULL, 0, &listenId);
	pubHandle = CreateThread(NULL, 0, &PublisherThread, NULL, 0, &pubHandleId);
	subHandle = CreateThread(NULL, 0, &SubscriberThread, NULL, 0, &subHandleId);
	// Initally Non-Signalized semaphore
	processingPub = CreateSemaphore(NULL, 0, 1, NULL);
	publishers = CreateSemaphore(NULL, 1, 0, NULL);
	subscribers = CreateSemaphore(NULL, 0, 1, NULL);
	processingSub = CreateSemaphore(NULL, 1, 0, NULL);
	processingThread = CreateThread(NULL, 0, &ProcessingThread, NULL, 0, &processingThreadId);

	ch = _getch();
	fflush(stdin);
	endFlag = false;
	// Time out is needed for threads to finish all actions and cleanup
	Sleep(5000);
	SafeExit();
	ch = _getch();
	fflush(stdin);
	WSACleanup();
	return 0;
}

void SafeExit()
{
	// Release semaphore so that threads unblocks
	ReleaseSemaphore(processingPub, 1, 0);
	ReleaseSemaphore(publishers, 1, 0);
	// Close all threads
	SAFE_CLOSE(processingPub);
	SAFE_CLOSE(processingThread);
	SAFE_CLOSE(listenHandle);
	SAFE_CLOSE(publishers);
	SAFE_CLOSE(pubHandle);
	SAFE_CLOSE(subHandle);
	SAFE_CLOSE(processingSub);
	SAFE_CLOSE(subscribers);
	// Free up memory
	DestroyPubs(&pubs);
	queue.DestroyQueue();
	DestroySubs(&allSubscribers);
	// Close Sockets
	shutdown(pubListenSocket, SD_BOTH);
	shutdown(subListenSocket, SD_BOTH);
	closesocket(pubListenSocket);
	closesocket(subListenSocket);
	WSACleanup();
}

BOOL CTRLHandler(DWORD event)
{
	if (event == CTRL_CLOSE_EVENT || event == CTRL_C_EVENT)
	{
		endFlag = false;
		SafeExit();
		Sleep(2000);
		return TRUE;
	}
	return FALSE;
}


