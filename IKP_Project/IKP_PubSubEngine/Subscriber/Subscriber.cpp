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
SOCKET connectSocket;
int res;

bool Communication(SOCKET*);
bool AppLogic(SOCKET*);
BOOL CTRLHandler(DWORD event);  // Handle Close event of a console
void SafeExit();				//Deinitialze all used data

int main(int argc, char* argv[])
{
	if (!InitializeWindowsSockets())
		return 1;
	//Set console Close event handle - Graceful shutdown
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CTRLHandler, TRUE);

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
	serverAddress.sin_port = htons(SUB_PORT);

	if ((connectSocket = Connect((sockaddr*)&serverAddress, sockAddrLen, &mode)) == INVALID_SOCKET)
	{
		printf("Unable to connect to server.\n");
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	int choice = -1;
	int topicCounter = 0;

	Topic topics[6];
	printf("\t\t*************************************************\n");
	printf("\t\t**                                             **\n");
	printf("\t\t**                                             **\n");
	printf("\t\t**                 SUBSCRIBER                  **\n");
	printf("\t\t**                                             **\n");
	printf("\t\t**                                             **\n");
	printf("\t\t*************************************************\n");

#pragma region User Input
	while (choice != 0)
	{
		printf("\nSelect Signal Type (1: Status, Any other number: Analog):  ");
		res = scanf("%d", &choice);
		if (choice == 1)
		{
			printf("Select Device Type (1: Fuse, 2: Breaker, Any other number: All):  ");
			res = scanf("%d", &choice);
			if (choice == 1)
			{
				Topic topic;
				topic.signal = Status;
				topic.device = Fuse;

				bool mayAdd = true;

				for (int i = 0; i < topicCounter; i++) {
					if ((topics[i].signal == topic.signal && topics[i].device == topic.device)
						|| (topics[i].signal == topic.signal && topics[i].device == All))
					{
						printf("--------You have already subscribed to this item!--------\n");
						mayAdd = false;
					}
				}

				if (mayAdd) {
					topics[topicCounter] = topic;
					++topicCounter;
				}

			}
			else if (choice == 2)
			{
				Topic topic;
				topic.signal = Status;
				topic.device = Breaker;

				bool mayAdd = true;

				for (int i = 0; i < topicCounter; i++) {
					if ((topics[i].signal == topic.signal && topics[i].device == topic.device)
						|| (topics[i].signal == topic.signal && topics[i].device == All))
					{
						printf("--------You have already subscribed to this item!--------\n");
						mayAdd = false;
					}
				}

				if (mayAdd) {
					topics[topicCounter] = topic;
					++topicCounter;
				}
			}
			else
			{
				Topic topic;
				topic.signal = Status;
				topic.device = All;

				bool mayAdd = true;

				for (int i = 0; i < topicCounter; i++) {
					if (topics[i].signal == topic.signal && topics[i].device == topic.device) {
						printf("--------You have already subscribed to this item!--------\n");
						mayAdd = false;
					}
				}

				if (mayAdd) {
					topics[topicCounter] = topic;
					++topicCounter;
				}
			}
		}
		else
		{
			printf("Select Signal Type (1: Sec_A, 2: Sec_V, Any other number: All):  ");
			res = scanf("%d", &choice);
			if (choice == 1)
			{
				Topic topic;
				topic.signal = Analog;
				topic.device = Sec_A;

				bool mayAdd = true;

				for (int i = 0; i < topicCounter; i++) {
					if ((topics[i].signal == topic.signal && topics[i].device == topic.device)
						|| (topics[i].signal == topic.signal && topics[i].device == All))
					{
						printf("--------You have already subscribed to this item!--------\n");
						mayAdd = false;
					}
				}

				if (mayAdd) {
					topics[topicCounter] = topic;
					++topicCounter;
				}
			}
			else if (choice == 2)
			{
				Topic topic;
				topic.signal = Analog;
				topic.device = Sec_V;

				bool mayAdd = true;

				for (int i = 0; i < topicCounter; i++) {
					if ((topics[i].signal == topic.signal && topics[i].device == topic.device)
						|| (topics[i].signal == topic.signal && topics[i].device == All))
					{
						printf("--------You have already subscribed to this item!--------\n");
						mayAdd = false;
					}
				}

				if (mayAdd) {
					topics[topicCounter] = topic;
					++topicCounter;
				}
			}
			else
			{
				Topic topic;
				topic.signal = Analog;
				topic.device = All;

				bool mayAdd = true;

				for (int i = 0; i < topicCounter; i++) {
					if (topics[i].signal == topic.signal && topics[i].device == topic.device) {
						printf("--------You have already subscribed to this item!--------\n");
						mayAdd = false;
					}
				}

				if (mayAdd) {
					topics[topicCounter] = topic;
					++topicCounter;
				}
			}
		}

		printf("Press 0 to stop subscribing or any other number to continue: ");
		res = scanf("%d", &choice);
	}
#pragma endregion


	if (!Subscribe(connectSocket, topics, topicCounter))
	{
		printf("Unable to subscibe to server.\n");
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

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
	FD_SET read;
	timeval timeVal;
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 0;

	FD_ZERO(&read);
	FD_SET(*connectSocket, &read);

	result = select(0, &read, NULL, NULL, &timeVal);
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
	else if (FD_ISSET(*connectSocket, &read))
	{
		return AppLogic(connectSocket);
	}
	return true;
}

bool AppLogic(SOCKET* s)
{
	int len = 0;
	bool result = CustomRecv(*s, (char*)&len, sizeof(int));
	if (result)
	{
		char* buffer = (char*)malloc(len);
		result = CustomRecv(*s, buffer, len);
		if (result)
		{
			// Print in format
			printf("\nRecieved: %d\n", len / sizeof(TopicChange));
			TopicChange* tps = (TopicChange*)buffer;
			//ex: -7s - seven places empty than print 
			printf("|%-7s|%-8s|%-5s|%-6s|\n", "Type", "Device", "Value", "RecvId");
			printf("-------------------------------\n");
			for (unsigned int i = 0; i < len / sizeof(TopicChange); i++)
			{
				if (tps != NULL)
					printf("|%-7s|%-8s|%-5.2lf|%-6d|\n", GetStringFromSignalType(tps[i].topic.signal), GetStringFromDeviceType(tps[i].topic.device), tps[i].value, i + 1);
			}
		}
		free(buffer);
	}

	return result;
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
	closesocket(connectSocket);
	WSACleanup();
}