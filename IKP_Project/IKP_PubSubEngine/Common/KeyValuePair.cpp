#include "pch.h"
#include "KeyValuePair.h"


/*!
* Get value for given key - (signal,device)
*
* @param dictionary		- Source dictionary
* @param signal			- Signal Type
* @param device			- Device Type
*
* @return				- List of Subscribers for given key - (signal,device)
*/
Subscriber* GetValue(KeyValuePair* dictionary, enum SignalType signal, enum DeviceType device)
{
	Topic topic;
	topic.signal = signal;
	topic.device = device;
	// Itterate and find key, when key is founr return data on that key
	for (int i = 0; i < 6; i++)
	{
		if (dictionary[i].topic.Equals(topic))
			return dictionary[i].subs;
	}
	return NULL;
}
