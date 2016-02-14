#include "skiplist.h"

#include <stdlib.h>
#include <stdio.h>


/*
initialize skiplist with tail=NULL
*/
void slInit()
{
	slHead = malloc( sizeof(slNode) + sizeof(slNode*)*slLEVELS );
	for(char i=0; i<slLEVELS; i++)	slHead->next[i] = NULL;
}

/* Insert key into skiplist.
return -1: newKey exists
return 0:  sucess, nothing left to do
return 1:  sucess, top-level reached (x-fast trie required)
*/
char slInsert(int newKey)
{
/*
1: find preceding bottom node by going through the nodes/level starting on top
2: add after that node
3: randomly add to higher levels

for each level (except bottom?):
	if curNode is smaller
		if nextNode != NULL && nextNode is smaller: curNode = nextNode
		else: go down
bottom: if nextNode = NULL or nextNode is bigger, become nextNode
if (coinflip): randomly add to higher level
	find previous node and next node that have a higher level
*/
	
	//slNode * curNode = slHead;
	slNode * curNode[slLEVELS];
	curNode[slLEVELS-1] = slHead;
	char lv;
	// create a path from top to bottom
	for(lv = slLEVELS-1; lv > 0; lv--)
	{
		while((*curNode[lv]).key < newKey)	// if curNode is smaller
		{
			if(curNode[lv]->next[lv] && (*curNode[lv]->next[lv]).key<newKey)
			{
				curNode[lv] = curNode[lv]->next[lv];
			}
			else	// keep track of closestNode and go down
			{
				curNode[lv-1] = curNode[lv];
				break;
			}
		}
	}
	for(;;)	// bottom reached; create and add node
	{
		if(!(curNode[lv]->next[lv]) || (*curNode[lv]->next[lv]).key>newKey)
		{ // become next node
		if(newKey == (*curNode[lv]).key)	return -1;
			int coin = rand() % (slLEVELS*2)+1;
			slNode * newNode = malloc(sizeof(slNode) +
					   sizeof(slNode*)*((slLEVELS*2)/coin));
			(*newNode).key = newKey;
			for(lv = 0; lv < slLEVELS; lv++)
			{

				if((coin) <= (slLEVELS*2)/(lv+1))
				{
					newNode->next[lv] = curNode[lv]->next[lv];
					curNode[lv]->next[lv] = newNode;
					if(lv == slLEVELS-1)	return 1;
				}
				else return 0;
			}
			break;
		}
		else curNode[lv] = curNode[lv]->next[lv];
	}
}

// return a pointer to the node which contains the desired key, or NULL if 404
slNode * slFind(int key)
{
	slNode * curNode = slHead;
	char lv = slLEVELS-1;
	for(;;)
	{
		if(!(*curNode).next[lv] || (*(*curNode).next[lv]).key>key)
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

/*
Remove node whose value matches key
return 0 if successful
return -1 if 404
*/
char slRemove(int key)
{
	char lv = slLEVELS-1;
	slNode * curNode[slLEVELS];
	slNode * target;
	curNode[lv] = slHead;
	for(;;)
	{
		// go down?
		if(!(curNode[lv]->next[lv]) || (*curNode[lv]->next[lv]).key>key)
		{
			if(lv)
			{
				curNode[lv-1] = curNode[lv--];
				continue;
			}
			return -1;
		}
		// target acquired?
		if((*(*curNode[lv]).next[lv]).key == key)
		{
			target = curNode[lv]->next[lv];	
			break;
		}
		// go right
		curNode[lv] = (*curNode[lv]).next[lv];
	}
	// eliminate target
	for(;lv >= 0; lv--)	curNode[lv]->next[lv] = target[lv].next[lv];
	free(target);
	return 0;
}

