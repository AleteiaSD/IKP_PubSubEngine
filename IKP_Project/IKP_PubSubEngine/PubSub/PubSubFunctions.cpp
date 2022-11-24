#include "pch.h"
#include "PubSubFunctions.h"

/*!
* Generate dictionary of every possible Topic combination - 6 of them
*
* @param dictionay		- Dictionay to add generated values
*/
void GenerateDictionaryItems(KeyValuePair* dictionary)
{
	KeyValuePair p1;
	p1.topic.signal = Status;
	p1.topic.device = Fuse;
	InitSubs(&(p1.subs));

	KeyValuePair p2;
	p2.topic.signal = Status;
	p2.topic.device = Breaker;
	InitSubs(&(p2.subs));

	KeyValuePair p3;
	p3.topic.signal = Status;
	p3.topic.device = All;
	InitSubs(&(p3.subs));

	KeyValuePair p4;
	p4.topic.signal = Analog;
	p4.topic.device = Sec_A;
	InitSubs(&(p4.subs));

	KeyValuePair p5;
	p5.topic.signal = Analog;
	p5.topic.device = Sec_V;
	InitSubs(&(p5.subs));

	KeyValuePair p6;
	p6.topic.signal = Analog;
	p6.topic.device = All;
	InitSubs(&(p6.subs));

	dictionary[0] = p1;
	dictionary[1] = p2;
	dictionary[2] = p3;
	dictionary[3] = p4;
	dictionary[4] = p5;
	dictionary[5] = p6;
}