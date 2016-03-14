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
	// Find a predecessor node on current level (numLv-1)
	// return NULL if key exists on lv >= numLv
	for(;;)
	{	
		nextNode = getPtr(curNode->next[lv]);
		if(!nextNode)	continue;
		// Move right until successor is found
		//if(!nextNode)	continue;
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
	for(;;)
	{
		slInsertion:
		nextNode = getPtr(curNode->next[lv]);
		if(!nextNode)	continue;
		// Move right until successor is found
		//if(!nextNode)	continue;
		if(nextNode->key < newKey) // remove nextnode
		{
			// Insert on the left of duplicate node.
			// To insert on the left, use <= instead.
			if(nextNode)//dbg
			//if(nextNode->stopflag)	continue; // remove
			curNode = nextNode; // do some checks? del
			continue;
		}
		// Merge nodes if exists on lower level
		if(nextNode->key == newKey)
		{
			//slNode * duplicateNode = nextNode;
			for(;lv >= 0; lv--)
			{
				// Mark next node for deletion
				while(atomic_fetch_or(&(nextNode->next[lv]), 1) & 1);
				newNode->next[lv] = (nextNode->next[lv]) & (UINTPTR_MAX ^ 1);
				while(!atomic_compare_exchange_strong(&(curNode->next[lv]), &nextNode, (uintptr_t)newNode))
				{
					if(getPtr(curNode->next[lv]) != nextNode)
						curNode = getPtr(curNode->next[lv]);
					//lv++;
					//continue;
				}
				//nextNode->next[lv] &= (UINTPTR_MAX-1);//dbg
				//while(!atomic_compare_exchange_strong(&(newNode->next[lv]), &nextNode, (uintptr_t)getPtr(nextNode->next[lv])));
			}
			free(nextNode->next);
			free(nextNode);
			if(numLv == slLEVELS-1)	return newNode;
			else	return NULL;
		}
		/*
			*/
			// duplicate node is on the right (1st found)
		/*for each level:
			mark the next node if matching key
			CAS(&(newNode->next[lv]), &duplicateNode, (uintptr_t)getPtr(target->next[lv])
		*/
				
			//printf("merging\n");
			//return slMerge(nextNode, newNode, curNode, lv);
			
			//slNode * duplicateNode = nextNode;
			//duplicateNode->stopflag = 1;
			/*
			for(;;)
			{
				if(getPtr(curNode->next[lv]) != duplicateNode)
				{
					curNode = getPtr(curNode->next[lv]
				}
			}
			
			if(!merging)
			{
				merging = lv;
				duplicateNode = nextNode;
				duplicateNode->stopflag = 1;
			}
			*/
			//slMerge(curNode, nextNode, lv, newNode, numLv);
			//goto endOfInsert;
		
		// Insert and go down (third? attempt)
		newNode->next[lv] = (uintptr_t)nextNode;
		while(!atomic_compare_exchange_strong(&curNode->next[lv], &nextNode, (uintptr_t)newNode))
		{
			// CAS failed because
			// curNode->next is marked for deletion
			// or
			// curNode->next does not point to nextNode
			
			// Check if curNode is marked for deletion:
			if(!curNode || curNode->next[lv] & 1)
			{
				printf("slInsert starting over\n");
				// Start lv over
				curNode = slHead;
				goto slInsertion;
			}
			// Check if curnext != next
			else if(getPtr(curNode->next[lv]) != nextNode)
			{
				// Update current node
				printf("slInsert updating curNode\n");
				while(getPtr(curNode->next[lv])->key < newKey)
				{
					curNode = getPtr(curNode->next[lv]);
				}
				nextNode = getPtr(curNode->next[lv]);
				continue;
			}
			printf("\nPanic\n");
			//printf("curNode (self/next/key): %u / %u / %u\nnextNode: %u / %u / %u\nKey: %u\nnewNode: %u / %u / %u\n");
		}
		
		if(lv)
		{	
			if(lv == slLEVELS-1)	newNode->previous = curNode;
			lv--;
		}
		else
		{
			if(numLv == slLEVELS-1)	return newNode;
			else	return NULL;
		}
	}	
		/*
		// Insert and go down
		// Mark next node
		if(atomic_fetch_or(&(curNode->next[lv]), 1) & 1) // If failed:
		{
			
			//printf("Insertion delayed: next is marked.\n");
			return slInsert(slHead, newKey);
			curNode = slHead;
			printf("%u, %u, %u\n",curNode->key, newKey, getPtr(curNode->next[lv])->key);
			if(curNode->stopflag)
			{
				curNode = slHead;	// replace w/findPredecessor function
				printf("for deletion\t");
			}
			continue;
		}
		else // Success: Insert and go down
		{
		//printf("Insert marked %u->%u\n", curNode->key, getPtr(curNode->next[lv])->key);
			// Verify that next is still valid
			if(getPtr(curNode->next[lv]) != nextNode)
			{
				printf("Reset flag.\n");
				// Reset flag
				curNode->next[lv] &= (UINTPTR_MAX-1);
				continue;
			}
			// Top? Set previous node.
			if(lv == slLEVELS-1)	newNode->previous = curNode;
			// Do the insertion:
			newNode->next[lv] = (atomic_uintptr_t)nextNode;
			curNode->next[lv] = (atomic_uintptr_t)newNode;
			if(lv)	lv--;
			else	break;
		}
	}
	
	endOfInsert:
	if(numLv == slLEVELS-1)	return newNode;
	else	return NULL;
	*/
	
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

slNode * slMerge(slNode * oldNode, slNode * newNode, slNode * curNode, int8_t oldLv)
{
	//printf("merging\n");
	oldNode->stopflag = 1;
	int8_t lv = oldLv;
	uint32_t key = newNode->key;
	slNode * tmpNode;
	for(;;)
	{
		if(getPtr(curNode->next[lv])->key < key)
		{
			curNode = getPtr(curNode->next[lv]);
			continue;
		}
		// cas 
		if(getPtr(curNode->next[lv])->key != key)
			printf("slMerge: Undefined behavior.\n");
		/*
		try: tmp=oldnext, oldnext points to new, new points to tmp
		then get each predecessor next to point to new (cas)
		*/
		tmpNode = getPtr(oldNode->next[lv]);
		oldNode->next[lv] = (atomic_uintptr_t)newNode;
		newNode->next[lv] = (atomic_uintptr_t)tmpNode;
		
		curNode->next[lv] = (atomic_uintptr_t)newNode;
		
		//newNode->next[lv] = oldNode->next[lv];
		//curNode->next[lv] = (atomic_uintptr_t)newNode;
		if(lv)
		{
			lv--;
			continue;
		}
		else
		{
			free(oldNode->next);
			free(oldNode);
			return (oldLv==slLEVELS-1)?newNode:NULL;
		}
	}
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

