#include "pch.h"
#include "interface.h"
#include "customCommunication.h"

/*!
* Function to connect to PubSubEngine
* @param address		- Address
* @param sockAddrLen    - Length of address
* @param mode			- Mode of connection
*
* @return				- Connection Socket
*/
SOCKET Connect(sockaddr* address, int sockAddrLen, unsigned long int* mode)
{
	SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connect(connectSocket, (sockaddr*)address, sockAddrLen) == SOCKET_ERROR)
	{
		printf("Unable to connect to server.\n");
		closesocket(connectSocket);
		WSACleanup();
		return INVALID_SOCKET;
	}

	if (ioctlsocket(connectSocket, FIONBIO, mode) == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %ld\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return INVALID_SOCKET;
	}

	return connectSocket;
}

/*!
* Function to Publish to PubSubEngine
* @param socket			- PubSubEngine socket
* @param head		    - Head of the list of changes
*
* @return				- Success of operation
*/
bool Publish(SOCKET socket, NODE* head)
{
	int size = count(head) * sizeof(TopicChange);
	TopicChange* devs = (TopicChange*)malloc(size);
	// Copy elements to send buffer
	for (int i = 0; i < count(head); i++)
		devs[i] = *(elementAt(head, i));
	// Send header, in header is length of data to be sent 
	bool result = CustomSend(socket, (char*)&size, sizeof(int));
	if (result)
	{
		// Send main data
		result = CustomSend(socket, (char*)devs, size);
		if (result)
		{
			printf("\nSuccess!\n");
		}
		free(devs);
	}
	return result;
}

/*!
* Function to Subscripe to Topic on PubSubEngine
* @param socket			- PubSubEngine socket
* @param topics		    - List of topics
* @param count			- Size of list of topics
*
* @return				- Success of operation
*/
bool Subscribe(SOCKET socket, Topic* topics, int count)
{
	int len = count * sizeof(Topic);
	// Send header, in header is length of data to be sent
	bool result = CustomSend(socket, (char*)&len, sizeof(int));
	if (result)
	{
		// Send main data
		result = CustomSend(socket, (char*)topics, len);
	}
	return result;
}