#pragma once
#include "data.h"
#include "topichangeList.h"

/*!
* Function to connect to PubSubEngine
* @param address		- Address 
* @param sockAddrLen    - Length of address
* @param mode			- Mode of connection
* 
* @return				- Connection Socket
*/
SOCKET Connect(sockaddr* address, int sockAddrLen, unsigned long int* mode);

/*!
* Function to Publish to PubSubEngine
* @param socket			- PubSubEngine socket
* @param head		    - Head of the list of changes
*
* @return				- Success of operation
*/
bool Publish(SOCKET socket, NODE* head);

/*!
* Function to Subscripe to Topic on PubSubEngine
* @param socket			- PubSubEngine socket
* @param topics		    - List of topics
* @param count			- Size of list of topics
* 
* @return				- Success of operation
*/
bool Subscribe(SOCKET socket, Topic* topics, int count);