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

/* Type of the Signal */
enum SignalType
{
	Analog = 0,
	Status
};

/* Type of the Device */
enum DeviceType
{
	Fuse = 0,
	Breaker,
	Sec_A,
	Sec_V,
	All
};

/* Structure of Topic, with Equals function implemented */
typedef struct topic_st
{
	SignalType signal;
	DeviceType device;

	bool Equals(topic_st t)
	{
		if (t.device == All)
			return signal == t.signal;
		return signal == t.signal && device == t.device;
	}

}Topic;

/* Structure of Change, includes Topic and value */
typedef struct topic_change
{
	Topic topic;
	double value;

}TopicChange;

/*!
* Enum to String conversion
*
* @param st		- Value of enum
*
* @return		- String representarion od enum
*/
const char* GetStringFromSignalType(enum SignalType st);

/*!
* Enum to String conversion
*
* @param st		- Value of enum
*
* @return		- String representarion od enum
*/
const char* GetStringFromDeviceType(enum DeviceType dt);