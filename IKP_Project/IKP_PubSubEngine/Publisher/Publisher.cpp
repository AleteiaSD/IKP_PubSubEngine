#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"Ws2_32.lib")

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "constants.h"
#include "interface.h"
#include "customCommunication.h"

int sockAddrLen = sizeof(struct sockaddr);
unsigned long int mode = 1;
bool endFlag = true;
SOCKET connectSocket = INVALID_SOCKET;
// List of topicChanges
NODE* head;
int res;
int fuses = 0, breakers = 0, as = 0, vs = 0;
int messageCountPerDevice = 0;
int timeOut = 0;

bool Communication(SOCKET*);
bool AppLogic(SOCKET*);
void GenerateDevices();			// Generate random Topic Changes
BOOL CTRLHandler(DWORD event);  // Handle Close event of a console
void SafeExit();				// Deinitialze all used data

int main(int argc, char* argv[])
{
	if (!InitializeWindowsSockets())
		return 1;

	// Set Console Close event handler - Graceful shutdown
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CTRLHandler, TRUE);


	// Default values
	timeOut = 2000;
	messageCountPerDevice = 5;
	fuses = messageCountPerDevice;
	breakers = messageCountPerDevice;
	as = messageCountPerDevice;
	vs = messageCountPerDevice;

	connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(DEFAULT_IP);
	serverAddress.sin_port = htons(PUB_PORT);

	if ((connectSocket = Connect((sockaddr*)&serverAddress, sockAddrLen, &mode)) == INVALID_SOCKET)
	{
		printf("Unable to connect to server.\n");
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	printf("\t\t*************************************************\n");
	printf("\t\t**                                             **\n");
	printf("\t\t**                                             **\n");
	printf("\t\t**                 PUBLISHER                   **\n");
	printf("\t\t**                                             **\n");
	printf("\t\t**                                             **\n");
	printf("\t\t*************************************************\n");

	init_list(&head);

	while (endFlag)
	{
		if (!Communication(&connectSocket))
			break;
	}

	SafeExit();

	return 0;
}

bool Communication(SOCKET* connectSocket)
{
	int result;
	FD_SET write;
	timeval timeVal;
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 0;

	FD_ZERO(&write);
	FD_SET(*connectSocket, &write);

	result = select(0, NULL, &write, NULL, &timeVal);
	if (result == SOCKET_ERROR)
	{
		printf("\nselect failed with error: %d\n", WSAGetLastError());
		return false;
	}
	else if (result == 0)
	{
		Sleep(SLEEP_TIME);
		return true;
	}
	else if (FD_ISSET(*connectSocket, &write))
	{
		return AppLogic(connectSocket);
	}
	return true;
}

bool AppLogic(SOCKET* s)
{
	Sleep(timeOut);
	GenerateDevices(); // generate changes
	bool result = Publish(*s, head);
	while (count(head) > 0)
	{
		// Remove changes one by one
		removeAt(&head, 0);
	}
	return result;
}

void GenerateDevices()
{
	for (int i = 0; i < breakers; i++)
	{
		TopicChange topicChange;
		topicChange.topic.signal = Status;
		topicChange.topic.device = Breaker;
		topicChange.value = rand() % 2;		// Digital Value 0 or 1
		add_on_begin(&head, topicChange);
	}

	for (int i = 0; i < fuses; i++)
	{
		TopicChange topicChange;
		topicChange.topic.signal = Status;
		topicChange.topic.device = Fuse;
		topicChange.value = rand() % 2;		// Digital Value 0 or 1
		add_on_begin(&head, topicChange);
	}

	for (int i = 0; i < as; i++)
	{
		TopicChange topicChange;
		topicChange.topic.signal = Analog;
		topicChange.topic.device = Sec_A;
		topicChange.value = rand() % 100;	// Analog Value btw. 0 and 100
		add_on_begin(&head, topicChange);
	}

	for (int i = 0; i < vs; i++)
	{
		TopicChange topicChange;
		topicChange.topic.signal = Analog;
		topicChange.topic.device = Sec_V;
		topicChange.value = rand() % 100;	// Analog Value btw. 0 and 100
		add_on_begin(&head, topicChange);
	}
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

void SafeExit()
{
	destroy_list(&head);
	closesocket(connectSocket);
	WSACleanup();
}