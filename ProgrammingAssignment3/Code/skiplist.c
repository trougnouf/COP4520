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
slNode * slInsert(slNode * slHead, uint32_t key)
{
	// Verify that new key is valid
	if(key >= MAXKEY || key <= MINKEY)	return NULL;
	
	// Determine # of levels
	uint8_t numLv = flipcoins()-1;
	
	// Initialize variables
	slNode * curNode = slHead;
	slNode * nextNode;
	int8_t lv;
	int8_t started = 0;
	
	// Initialize new node
	slNode * newNode = malloc(sizeof(slNode));
	newNode->next = malloc(sizeof(atomic_uintptr_t)*(numLv+1));
	newNode->key = key;
	
	// Go through each level
	for(lv=slLEVELS-1;lv>=0;lv--)
	{
		slInserting:	// Come back without decrementing lv

		// Move right (this crashes)
		/*
		while(key > getPtr(curNode->next[lv])->key)
		{
			curNode = getPtr(curNode->next[lv]);
		}
		//if(curNode->next[lv] & 1)	goto slInserting;
		nextNode = getPtr(curNode->next[lv]);
		*/
		
		nextNode = getPtr(curNode->next[lv]);
		if(!nextNode)	goto slInserting;
		// Move right until successor is found
		if(nextNode->key < key)
		{
			curNode = nextNode; // do some checks? del
			goto slInserting;
		}
		
		// Duplicate found
		if(nextNode->key == key)
		{
			// Insertion not started? Abort
			if(!started)
			{
				//printf("Abort\n");
				free(newNode->next);
				free(newNode);
				return NULL;
			}
			
			// Insertion started? Merge
			
			// Mark node for deletion
			if(!(atomic_fetch_or(&(nextNode->next[lv]), 1) & 1))
			{
				//printf("Merge: bit stealing failed.\n");
				// Already marked
				//curNode = slHead; // dbg
				goto slInserting;
			}
			newNode->next[lv] = (nextNode->next[lv]) & (UINTPTR_MAX ^ 1);
			if(!atomic_compare_exchange_strong(&(curNode->next[lv]), &nextNode, (uintptr_t)newNode))
			{
				//printf("Merge: CAS failed (%u).\n", key);
				// CAS failed. Reset flag and start over.
				//if(nextNode->next[lv]) // dbg, prob useless
				nextNode->next[lv] &= (UINTPTR_MAX ^ 1);
				//curNode = slHead; // dbg
				goto slInserting;
			}
			if(lv)
			{
				if(lv == slLEVELS-1)
					newNode->previous = curNode;
				continue;
			}
			
			//printf("Freeing %u\n", nextNode->key);
			free(nextNode->next);
			free(nextNode);
			break;
			
		}
		
		// Insert
		if(lv <= numLv)
		{
			newNode->next[lv] = (uintptr_t)nextNode;
			if(!atomic_compare_exchange_strong(&(curNode->next[lv]), &nextNode, (uintptr_t)newNode))
			{
				// Insert failed: start over.
				printf("Insert failed: starting over (%u).\n", key);
				//curNode = slHead; // dbg
				goto slInserting;
			}
			started = 1;
			if(lv == slLEVELS-1)	newNode->previous = curNode;
		}
	}
	
	if(numLv == slLEVELS-1)	return newNode;
	else	return NULL;
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
	
	// Find target
	slRemoveFindTarget:
	// Go right
	while(key > getPtr(curNode->next[lv])->key)
	{
		curNode = getPtr(curNode->next[lv]);
	}
	/*
	if((getPtr(curNode->next[lv])->key==MAXKEY)|| getPtr(curNode->next[lv])->key > key)
	{
		if(lv)
		{
			lv--;
			goto slRemoveFindTarget;
		}
		return -1;
	}
	*/
	// Go down
	if(getPtr(curNode->next[lv])->key != key)
	{
		if(lv)
		{
			lv--;
			goto slRemoveFindTarget;
		}
		else return -1;	// 404
	}
	
	// Key found
	else //if(getPtr(curNode->next[lv])->key == key)
	{	
		if(lv == slLEVELS-1)	removedFromTop = 1;
		target = getPtr(curNode->next[lv]);
		target->stopflag = 1;
	}
	
	// Remove target
	slRemoveNext:
	// Mark target->next (optional? check later)
	while(atomic_fetch_or(&(target->next[lv]), 1) & 1);
	printf("Remove marked %u->%u\n", target->key, getPtr(target->next[lv])->key);
	
	while(!atomic_compare_exchange_strong(&(curNode->next[lv]), &target, (uintptr_t)getPtr(target->next[lv])))
	{
		printf("Remove unmarked %u->%u\n", target->key, getPtr(target->next[lv])->key);
		target->next[lv] &= (UINTPTR_MAX-1);//dbg
		/* Reasons for failure:
			current->next[lv] is marked (try again),
			current is no longer predecessor
				(current = getPtr(current->next[lv])) */
		while(getPtr(curNode->next[lv]) != target)
			curNode = getPtr(curNode->next[lv]);
		
	}
	printf("Remove unmarked %u->%u\n", target->key, getPtr(target->next[lv])->key);
	target->next[lv] &= (UINTPTR_MAX-1);//dbg
	if(lv)
	{
		if(lv == slLEVELS-1)
			getPtr(target->next[lv])->previous = curNode;
		lv--;
		goto slRemoveNext;
	}
	else
	{
		free(target->next);
		free(target);
		return removedFromTop?1:0;
	}

/*		
		
		if getPtr(current->next[lv]) != target
			unmark current->next[lv]
			current = getPtr(current->next[lv]);
			try again
		else (same): current->next[lv] = target->next[lv]

*/

/*
// This was working (=not crashing)
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
				free(target->next);
				free(target);
				return removedFromTop?1:0;
			}
		}
		curNode=getPtr(curNode->next[lv]);	// go right
	}
	*/
}

// Return a node's predecessor on any given level
/*
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
*/

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

