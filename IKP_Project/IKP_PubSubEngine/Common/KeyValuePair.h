#pragma once

#include "data.h"
#include "subscriber.h"

/* Structue of one element in Dictionary<Topic,List<Subscriber>> */
typedef struct keyvaluepair_st
{
	Topic topic;
	Subscriber* subs;
}KeyValuePair;

/*!
* Get value for given key - (signal,device)
*
* @param dictionary		- Source dictionary
* @param signal			- Signal Type
* @param device			- Device Type
*
* @return				- List of Subscribers for given key - (signal,device)
*/
Subscriber* GetValue(KeyValuePair* dictionary, enum SignalType signal, enum DeviceType device);
