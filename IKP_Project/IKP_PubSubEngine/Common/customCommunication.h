#pragma once
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include "conio.h"
#include "constants.h"

/*!
* Sends data over TCP
*
* @param s		- Socket
* @param data	- Data to send
* @param size	- Size of data to send
*
* @return		- Success of operation
*/
bool CustomSend(SOCKET s, char* data, int size);

/*!
* Recieves data over TCP
*
* @param s		- Socket
* @param data	- Space for received data
* @param size	- Size of data to be recieved
*
* @return		- Success of operation
*/
bool CustomRecv(SOCKET s, char* data, int size);

/*!
* Initialize WinSock Lib
* 
* @return		- Success of operation
*/
bool InitializeWindowsSockets();
