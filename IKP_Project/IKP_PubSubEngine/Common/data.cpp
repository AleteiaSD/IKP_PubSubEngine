#include "pch.h"
#include "data.h"
/*!
* Enum to String conversion
*
* @param st		- Value of enum
*
* @return		- String representarion od enum
*/
const char* GetStringFromSignalType(enum SignalType st)
{
	const char* strings[] = { "Analog", "Status" };

	return strings[st];
}

/*!
* Enum to String conversion
*
* @param st		- Value of enum
*
* @return		- String representarion od enum
*/
const char* GetStringFromDeviceType(enum DeviceType dt)
{
	const char* strings[] = { "Fuse", "Breaker", "Sec_A", "Sec_V", "All" };

	return strings[dt];
}