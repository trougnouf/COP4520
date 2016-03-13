#include "skiplist.h"
#include "testcase1.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>

slNode * getPtr(uintptr_t p)	
{
	return (slNode *)(p & (UINTPTR_MAX ^ 1));
}

/*
initialize skiplist with tail as a node
*/
slNode * slInit()
{
	slNode * slHead = malloc( sizeof(slNode));
	slHead->next = malloc(sizeof(atomic_uintptr_t)*slLEVELS );
	slNode * slTail = malloc( sizeof(slNode));
	for(uint8_t i=0; i<slLEVELS; i++)
		slHead->next[i] = (atomic_uintptr_t)slTail;
	slTail->previous = slHead;
	slHead->key = MINKEY;
	slTail->key = MAXKEY;
	return slHead;
}

// return a pointer to the node which contains the desired key, or NULL if 404
slNode * slFind(slNode * slHead, uint32_t key)
{
	slNode * curNode = slHead;
	uint8_t lv = slLEVELS-1;
	for(;;)
	{
		if(!(getPtr(curNode->next[lv])->key==MAXKEY) || getPtr(curNode->next[lv])->key > key)
		{
			if(lv)
			{
				lv--;
				continue;
			}
			return NULL;
		}
		if(getPtr(curNode->next[lv])->key == key)
			return getPtr(curNode->next[lv]);	// found
		curNode=getPtr(curNode->next[lv]);	// go right
	}
}

/* Insert key into skiplist.
Return pointer to new node if top level reached, NULL otherwise.
slHead parameter should be xtrie findPredecessor
*/
slNode * slInsert(slNode * slHead, uint32_t newKey)
{
	// Verify that new key is valid
	if(newKey >= MAXKEY || newKey <= MINKEY)	return NULL;
	
	// Determine # of levels
	uint8_t numLv = flipcoins()-1;
	
	slNode * curNode = slHead;
	slNode * nextNode;
	int8_t lv = slLEVELS-1;
	// Find predecessor node on current level (numLv-1)
	for(;;)
	{	
		nextNode = getPtr(curNode->next[lv]);
		// Move right until successor is found
		if(nextNode->key < newKey)
		{
			curNode = nextNode; // do some checks? del
			continue;
		}
		// Move down until insertion level begins
		if(lv != numLv)
		{
			lv--;
			continue;
		}
		// Found 1st successor. Check if key exists, check/set flag
		else // lv == numLv
		{
			// Key exists on lv? No insert
			if(nextNode->key == newKey)	return NULL;
			break;
			/*
			// Flag next pointer or try again
			if(atomic_fetch_or(&(curNode->next[lv]), 1) & 1)
				continue;
			else
			{
				// Verify that next is still valid
				if(getPtr(curNode->next[lv]) != nextNode)
				{
					// Reset flag
					curNode->next[lv] ^ 1;
					continue;
				}
				break;
			}
			*/
		}
	}
	
	// Create and insert new node.
	// - Check for duplicate (merge)
	// Create new node
	slNode * newNode = malloc(sizeof(slNode));
	newNode->next = malloc(sizeof(atomic_uintptr_t)*(numLv+1));
	newNode->key = newKey;
	uint8_t merging = 0;
	slNode * duplicateNode;
	for(;;)
	{
		nextNode = getPtr(curNode->next[lv]);
		// Move right until successor is found
		if(nextNode->key < newKey)
		{
			curNode = nextNode; // do some checks? del
			continue;
		}
		// Merge nodes if exists on lower level
		if(nextNode->key == newKey)
		{
			for(;;)
			{break;
			}
			if(!merging)
			{
				merging = lv;
				duplicateNode = nextNode;
				duplicateNode->stopflag = 1;
			}
			//slMerge(curNode, nextNode, lv, newNode, numLv);
			//goto endOfInsert;
		}
		// Insert and go down
		// Mark next node
		if(atomic_fetch_or(&(curNode->next[lv]), 1) & 1) // If failed:
		{
			continue;
		}
		else // Success: Insert and go down
		{
			// Verify that next is still valid
			if(getPtr(curNode->next[lv]) != nextNode)
			{
				// Reset flag
				curNode->next[lv] ^ 1;
				continue;
			}
			// Top? Set previous node.
			if(lv == slLEVELS-1)	newNode->previous = curNode;
			// Do the insertion:
			if(merging) // Point to the next node
			{
				//if(atomic_fetch_or(&(nextNode->next[lv]), 1) & 1)
					//continue;
				newNode->next[lv] = duplicateNode->next[lv] ;
			}
			else	newNode->next[lv] = (atomic_uintptr_t)nextNode;
			curNode->next[lv] = (atomic_uintptr_t)newNode;
			if(lv)	lv--;
			else	break;
		}
	}
	
	endOfInsert:
	if(merging)
	{
		free(duplicateNode->next);
		free(duplicateNode);
	}
	if(numLv == slLEVELS-1)	return newNode;
	else	return NULL;
	
	
	/*
	// Step 1:	Find bottom node to insert on the right of, keeping
	//		track of every node on the way down
	slNode * curNode[slLEVELS];
	uint8_t lv = slLEVELS-1;
	curNode[lv] = slHead;
	for(;;)
	{
		// If next node is larger: go down. Else: move right.
		if(curNode[lv]->next[lv]->key > newKey)
		{
			if(lv)
			{
				lv--;
				curNode[lv] = curNode[lv+1];
				continue;
			}
			break;	// All the way down -> done
		}
		if(curNode[lv]->next[lv]->key == newKey)
			return NULL;	// Key already exists
		curNode[lv]= curNode[lv]->next[lv];	// go right
	}
	// Step 2: Insert on the right of curNode for each level
	slNode * newNode = malloc(sizeof(slNode));
	newNode->next = malloc(sizeof(slNode*)*(numLv));
	newNode->key = newKey;
	slNode * tmpnextnode;
	for(lv=0; lv < numLv; lv++)	// Insert from the bottom up
	{
		//check stop flag:
		if(curNode[lv]->next[lv]->stopflag)
			curNode[lv]->next[lv] = findPredecessor(curNode, lv,
			 newKey);
		// Using Harris' solution: 
		// make newnode's next pointer point to curnode's next node
		tmpnextnode = curNode[lv]->next[lv];
		newNode->next[lv] = tmpnextnode;
		// make curnode's next pointer point to new node iff curnode
		// hasn't changed, else start over.
		if(!atomic_compare_exchange_strong(&curNode[lv]->next[lv],
		  &tmpnextnode, newNode))
			lv--;	// start over	
	}
	if(numLv == slLEVELS)
	{
		newNode->previous = curNode[lv-1];
		return NULL;
	}
	return newNode;
	*/
}



/*
Remove node whose value matches key
return 1 if removed from top level
return 0 if successful
return -1 if 404
*/
int8_t slRemove(slNode * slHead, uint32_t key)
{
	int8_t removedFromTop;
	slNode * curNode = slHead;
	slNode * target;
	uint8_t lv = slLEVELS-1;
	for(;;)
	{
		if(!(getPtr(curNode->next[lv])->key==MAXKEY)|| getPtr(curNode->next[lv])->key > key)
		{
			if(lv)
			{
				lv--;
				continue;
			}
			return -1;
		}
		if(getPtr(curNode->next[lv])->key == key)
		{
			if(lv == slLEVELS-1)	removedFromTop = 1;
			target = getPtr(curNode->next[lv]);
			target->stopflag = 1;
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
		curNode=getPtr(curNode->next[lv]);	// go right
	}
}

// Return a node's predecessor on any given level
slNode * findPredecessor(slNode ** predecessors, uint8_t lv, uint32_t value)
{
	printf("Finding predecessor");
	// Find first predecessor which isn't set for deletion
	uint8_t curLv = lv;
	slNode * predecessor;
	do {
		if(curLv)
		{
			curLv--;
			predecessor = predecessors[curLv];
		}
		// every top node has a predecessor
		else	predecessor = predecessor->previous;
	} while(!predecessor || predecessor->stopflag);
	
	// Go right if this is still a predecessor, go up if stop flag detected.
	slNode * tmpNode = predecessor;
	while(getPtr(tmpNode->next[lv])->key < value)
	{
		tmpNode = getPtr(tmpNode->next[lv]);
		if(tmpNode && !(tmpNode->stopflag))	predecessor = tmpNode;
			
		else // tmpNode is being deleted
		{
			// try to get tmpNode's node if tmpNode still exists
			slNode * markedNode = getPtr(tmpNode->next[lv]);	//potential segfault if dne
			if(markedNode && !markedNode->stopflag)
			{
				if(markedNode->key > value)	break;
				else
				{
					tmpNode = markedNode;
					predecessor = markedNode;
					continue;
				}
			}
			
			// otherwise set tmpNode back to last working predecessor
			else	tmpNode= predecessor; 
		}
	}
	return predecessor;
}

// Wouldn't it be nice if this was commented? }:‑)
// Returns a number between 1 and slLEVELS
uint8_t flipcoins()
{
	int32_t coin = rand();
	int32_t shift = 1;
	uint8_t result = 0;
	for(uint8_t i=0; i<slLEVELS-1; i++)
	{
		if(coin&shift)	result++;
		else		break;
		shift<<=1;
	}
	return (result>=slLEVELS)?slLEVELS:result+1; // TODO remove redundant condition?
}

