#include "skiplist.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>


/*
initialize skiplist with tail=NULL
*/
slNode * slInit()
{
	slNode * slHead = malloc( sizeof(slNode));
	slHead->next = malloc(sizeof(slNode*)*slLEVELS );
	for(uint8_t i=0; i<slLEVELS; i++)	slHead->next[i] = NULL;
	return slHead;
}

// return a pointer to the node which contains the desired key, or NULL if 404
slNode * slFind(slNode * slHead, int key)
{
	slNode * curNode = slHead;
	uint8_t lv = slLEVELS-1;
	for(;;)
	{
		if(!curNode->next[lv] || curNode->next[lv]->key > key)
		{
			if(lv)
			{
				lv--;
				continue;
			}
			return NULL;
		}
		if((*(*curNode).next[lv]).key == key)
			return (*curNode).next[lv];	// found
		curNode=(*curNode).next[lv];	// go right
	}
}

/* Insert key into skiplist.
return -1: newKey exists
return 0:  sucess, nothing left to do
return 1:  sucess, top-level reached (x-fast trie required)
*/
char slInsert(slNode * slHead, int newKey)
{
	// Step 1: Find bottom node to insert on the right of
	slNode * curNode[slLEVELS];
	curNode[slLEVELS-1] = slHead;
	uint8_t lv = slLEVELS-1;
	for(;;)
	{
		if(!curNode[lv]->next[lv] || curNode[lv]->next[lv]->key > newKey)
		{
			if(lv)
			{
				lv--;
				curNode[lv] = curNode[lv+1];
				continue;
			}
			break;	// All the way down
		}
		if(curNode[lv]->next[lv]->key == newKey)
			return -1;	// found
		curNode[lv]= curNode[lv]->next[lv];	// go right
	}
	// Step 2: Insert on the right of curNode for each level
	uint8_t numLv = flipcoins();
	slNode * newNode = malloc(sizeof(slNode));
	newNode->next = malloc(sizeof(slNode*)*(numLv));
	newNode->key = newKey;
	for(lv=0; lv < numLv; lv++)
	{
		// Using Harris' solution: 
		// make newnode's next pointer point to curnode's next node
		slNode * tmpnextnode = curNode[lv]->next[lv];
		newNode->next[lv] = tmpnextnode;
		// make curnode's next pointer point to new node iff curnode
		// hasn't changed, else start over.
		if(!atomic_compare_exchange_strong(&curNode[lv]->next[lv], &tmpnextnode, newNode))
			lv--;	// start over	
	}
	if(numLv == slLEVELS)
	{
		newNode->previous = curNode[lv-1];
		return 1;
	}
	return 0;
}

/*
Remove node whose value matches key
return 1 if removed from top level
return 0 if successful
return -1 if 404
*/
char slRemove(slNode * slHead, int key)
{
	char removedFromTop;
	slNode * curNode = slHead;
	slNode * target;
	uint8_t lv = slLEVELS-1;
	for(;;)
	{
		if(!curNode->next[lv] || curNode->next[lv]->key > key)
		{
			if(lv)
			{
				lv--;
				continue;
			}
			return -1;
		}
		if(curNode->next[lv]->key == key)
		{
			if(lv == slLEVELS-1)	removedFromTop = 1;
			target = curNode->next[lv];
			target->removing = 1;
			curNode->next[lv] = target->next[lv];
			if(lv)
			{
				lv--;
				continue;
			}
			else
			{
				free(target);
				return removedFromTop?1:0;
			}
		}
		curNode=(*curNode).next[lv];	// go right
	}
}

uint8_t flipcoins()
{
	int coin = rand();
	int shift = 1;
	uint8_t result = 0;
	for(uint8_t i=0; i<32; i++)
	{
		if(coin&shift)	result++;
		else		break;
		shift<<=1;
	}
	
	return (result>=slLEVELS)?slLEVELS:result+1;
}

