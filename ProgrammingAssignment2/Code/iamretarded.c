/*
Sandbox
*/

//#include "testcase1.h"

//#include "test2.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define slLEVELS 5

typedef struct slNode_ {
	int key;
	struct slNode_ ** next;	// pointer to variable array
	struct slNode_ * previous;
} slNode;

int main()
{
	slNode * slHead = malloc( sizeof(slNode));
	slHead->next = malloc(sizeof(slNode*)*slLEVELS);
	
	slNode * anode = malloc( sizeof(slNode));
	anode->next = malloc(sizeof(slNode*)*slLEVELS );
	
	slNode * bnode = malloc( sizeof(slNode));
	bnode->next = malloc(sizeof(slNode*)*slLEVELS );
	slNode * cnode = malloc( sizeof(slNode));
	cnode->next = malloc(sizeof(slNode*)*slLEVELS );
	slNode * dnode = malloc( sizeof(slNode));
	dnode->next = malloc(sizeof(slNode*)*slLEVELS );
	slNode * enode = malloc( sizeof(slNode));
	enode->next = malloc(sizeof(slNode*)*slLEVELS );
	slNode * fnode = malloc( sizeof(slNode));
	fnode->next = malloc(sizeof(slNode*)*slLEVELS );
	slHead->next[0] = anode;
	anode->previous = slHead;
	anode->next[0] = bnode;
	anode->next[1] = cnode;
	anode->next[2] = dnode;
	printf("%d %d\n", slHead, anode->previous);
}

