#pragma once
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include "data.h"

/* Structure of one node in TopicChange list
 * value - TopicChange
 * next  - Pointer to next item in a list
 */
typedef struct node_st {
	TopicChange value;
	struct node_st* next;
}NODE;

/*!
* Initialize List of Nodes
*
* @param head		- Head of the list
*/
void init_list(NODE** head);

/*!
* Deinitialize List of Nodes
*
* @param head		- Head of the list
*/
void destroy_list(NODE** head);

/*!
* Prints List of Nodes
*
* @param head		- Head of the list
*/
void print_list(NODE* head);

/*!
* Allocate memory for one node
*
* @param		- TopicChange value
*
* @return 		- Pointer to created Node
*/
NODE* make_node(TopicChange value);

/*!
* Allocates new node and node on the begin of the list
*
* @param head		- Head of the list
* @param value		- Value of TopicChage
*/
void add_on_begin(NODE** head, TopicChange value);


/*!
* Allocates new node and node on the end of the list
*
* @param head		- Head of the list
* @param value		- Value of TopicChage
*/
void add_on_end(NODE** head, TopicChange value);

/*!
* Returns element at the specific index, if exist
*
* @param head		- Head of the list
* @param index		- Value of index
*
* @return			- Pointer to specific TopicChange
*/
TopicChange* elementAt(NODE* head, int index);

/*!
* Returns element at the specific index, if exist
*
* @param head		- Head of the list
* @param index		- Value of index
* @param value		- Value of TopicChange
*/
void insert(NODE** head, int index, TopicChange value);

/*!
* Removes element at the specific index
*
* @param head		- Head of the list
* @param index		- Value of index
*/
void removeAt(NODE** head, int index);

/*!
* Returns number of elements in a list
*
* @param head		- Head of the list
*
* @return			- Number of elements
*/
int count(NODE* head);