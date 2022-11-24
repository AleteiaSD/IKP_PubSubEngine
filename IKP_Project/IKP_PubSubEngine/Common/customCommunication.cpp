#include "pch.h"
#include "customCommunication.h"

/*!
* Sends data over TCP
*
* @param s		- Socket
* @param data	- Data to send
* @param size	- Size of data to send
*
* @return		- Success of operation
*/
bool CustomSend(SOCKET s, char* data, int size)
{
	int sent_size = 0;
	FD_SET write;
	timeval timeVal;
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 0;

	// Logic: Send until sends "size" bytes
	while (sent_size < size)
	{
		FD_ZERO(&write);
		FD_SET(s, &write);
		int result = select(0, NULL, &write, NULL, &timeVal);
		if (result == SOCKET_ERROR)
		{
			printf("\nselect failed with error: %d\n", WSAGetLastError());
			return false;
		}
		else if (result == 0)
		{
			Sleep(SLEEP_TIME);
			continue;
		}
		else if (FD_ISSET(s, &write))
		{
			result = send(s, data + sent_size, size - sent_size, 0);
			if (result == SOCKET_ERROR)
			{
				printf("\nsend failed with error: %d\n", WSAGetLastError());
				return false;
			}
			else if (result == 0 && size > 0)
			{
				return false;
			}
			else
			{
				sent_size += result;
			}
		}
	}
	return true;
}

/*!
* Recieves data over TCP
*
* @param s		- Socket
* @param data	- Space for received data
* @param size	- Size of data to be recieved
*
* @return		- Success of operation
*/
bool CustomRecv(SOCKET s, char* data, int size)
{
	int recvd_size = 0;
	FD_SET read;
	timeval timeVal;
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 0;

	// Logic: Recieve until recieves "size" bytes
	while (recvd_size < size)
	{
		FD_ZERO(&read);
		FD_SET(s, &read);
		int result = select(0, &read, NULL, NULL, &timeVal);
		if (result == SOCKET_ERROR)
		{
			printf("\nselect failed with error: %d\n", WSAGetLastError());
			return false;
		}
		else if (result == 0)
		{
			Sleep(SLEEP_TIME);
			continue;
		}
		else if (FD_ISSET(s, &read))
		{
			result = recv(s, data + recvd_size, size - recvd_size, 0);
			if (result == SOCKET_ERROR)
			{
				printf("\nrecv failed with error: %d\n", WSAGetLastError());
				return false;
			}
			else if (result == 0 && size > 0) 
			{
				return false;
			}
			else
			{
				recvd_size += result;
			}
		}
	}
	return true;
}

bool InitializeWindowsSockets()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return false;
	}
	return true;
}