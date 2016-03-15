#include "skiplist.h"
#include "testcase1.h"	// optional

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>

/*
	A fine concurrent skiplist
	May contain shattered dreams and rainbow unicorns.
	Not suitable for children and antelopes.
	Side effects may include shocking hallucinations and polite howling
	Run as follow using a recent version of GCC (>=5):
	
	gcc -pthread handler.c testcase1.c skiplist.c xtrie.c -std=gnu11 -O2 && ./a.out
	
	This skip list can be used independently (skiplist.c / skiplist.h). It
	is recommended to use a larger number of slLEVELS (16) when running 
	without the x-fast trie.
*/

/*
initialize skiplist with tail as a node (as opposed to a NULL pointer)
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
	// Initialize variables
	slNode * curNode = slHead;
	slNode * nextNode;
	int8_t lv;
	uint8_t chaosmeter = 0;
	
	// Go through each level
	for(lv=slLEVELS-1;lv>=0;lv--)
	{
		slFinding:	// Come back without decrementing lv
		
		if(chaosmeter >= MAXCHAOS )
		{
			#if DBG_WARNINGS >= 2
			printf("(F%6u) Maximum chaos reached, resetting chaosmeter and curNode\n", key);
			#endif
			chaosmeter = 0;
			curNode = slHead;
			#if DBG_PRINTSL == 2
			slPrint(slHead);
			#endif
		}

		nextNode = getPtr(curNode->next[lv]);
		if(!nextNode)
		{
			chaosmeter++;
			goto slFinding;
		}
		// Move right until successor is found
		if(nextNode->key < key)
		{
			curNode = nextNode; // do some checks? del
			goto slFinding;
		}
		
		// Found key
		if(nextNode->key == key)
		{
			return nextNode;
		}
	}
	return NULL;

}

/* Insert key into skiplist.
Return pointer to new node if top level reached, NULL otherwise.
slHead parameter should be xtrie findPredecessor
*/
slNode * slInsert(slNode * slHead, uint32_t key)
{
	// Verify that new key is valid
	if(key >= MAXKEY || key <= MINKEY)	return NULL;
	#if slMERGEDUPLICATES == 0
	if(slFind(slHead, key))	return NULL;
	#endif
	
	// Determine # of levels
	uint8_t numLv = flipcoins()-1;
	
	// Initialize variables
	slNode * curNode = slHead;
	slNode * nextNode;
	int8_t lv;
	int8_t started = 0;
	uint8_t chaosmeter = 0;
	
	// Initialize new node
	slNode * newNode = malloc(sizeof(slNode));
	newNode->next = malloc(sizeof(atomic_uintptr_t)*(numLv+1));
	newNode->key = key;
	
	// Go through each level
	for(lv=slLEVELS-1;lv>=0;lv--)
	{
		slInserting:	// Come back without decrementing lv
		
		if(chaosmeter >= MAXCHAOS )
		{
			#if DBG_WARNINGS >= 2
			printf("(I%6u) Maximum chaos reached, resetting chaosmeter and curNode.\n", key);
			#endif
			chaosmeter = 0;
			curNode = slHead;
			#if DBG_PRINTSL == 2
			slPrint(slHead);
			#endif
		}
		
		// Move right
		nextNode = (slNode *)(curNode->next[lv] & (UINTPTR_MAX ^ 1));
		
		if(!nextNode)
		{
			chaosmeter++;
			goto slInserting;
		}
		
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
			
			#if slMERGEDUPLICATES == 1
			// Insertion started? Merge
			// Mark node for deletion
			
			if(atomic_fetch_or(&(nextNode->next[lv]), 1) & 1)
			{
			
				#if DBG_WARNINGS >= 3
				printf("Merge: bit stealing failed (%u).\n",key);
				#endif
				// Already marked for deletion
				chaosmeter++;
				//continue;
				//goto slInserting;
			}
			
			nextNode->stopflag = 1;
			newNode->next[lv] = (nextNode->next[lv]) & (UINTPTR_MAX ^ 1);

			if(!atomic_compare_exchange_strong(&(curNode->next[lv]),
			   &nextNode, (uintptr_t)newNode))
			{
				// Don't want misaligned local pointer
				nextNode = (slNode *)((atomic_uintptr_t)nextNode&(UINTPTR_MAX ^ 1));
			
				#if DBG_WARNINGS >= 3
				printf("Merge: CAS failed (cur:%u,nxt:%u, k:%u).\n",
				       curNode->next[lv],nextNode,key);
				#endif
				// CAS failed. Reset flag and start over.
				//if(nextNode->next[lv]) // dbg, prob useless
				
				nextNode->next[lv] &= (UINTPTR_MAX ^ 1);
				
				chaosmeter++;
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
			#endif
		}
		
		// Insert
		if(lv <= numLv)
		{
		

			newNode->next[lv] = (uintptr_t)nextNode;
			
			if(!atomic_compare_exchange_strong(&(curNode->next[lv]),
			   &nextNode, (uintptr_t)newNode))
			{
				// Don't want misaligned local pointer
				nextNode = (slNode *)((atomic_uintptr_t)nextNode&(UINTPTR_MAX ^ 1));
				// Insert failed: start over.
				#if DBG_WARNINGS >= 3
				printf("Insert: CAS failed (curnxt:%u,nxt:%u, k:%u).\n"
				       , curNode->next[lv],nextNode,key);
				#endif
				chaosmeter++;
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
	int8_t removedFromTop = 0;
	slNode * curNode = slHead;
	slNode * nextNode;
	slNode * target;
	int8_t lv;
	int8_t started = 0;
	uint8_t chaosmeter = 0;
	uint8_t hopelesness = 0;
	uint8_t acceptance = 0;
	
	
	// Find target
	for(lv = slLEVELS-1; lv >= 0; lv--)
	{
		slRemoving:

		if(chaosmeter >= MAXCHAOS )	// where the fun begins
		{
			hopelesness++;
			#if DBG_WARNINGS >= 2
			printf("(R%6u) Maximum chaos reached, resetting chaosmeter and curNode.\n", key);
			#endif
			chaosmeter = 0;
			curNode = slHead;
			#if DBG_PRINTSL == 2
			slPrint(slHead);
			#endif
			if(hopelesness >= MAXCHAOS)
			{
				// This is useless most of the time.
				#if DBG_WARNINGS >= 1
				printf("Hopelesness reached. Unmarking target's next pointers. Undefined behavior is imminent.\n:)\n");
				printf("Looking for %u: %u (target=%u)\n", key,
				       slFind(slHead, key), target);
				#endif
				target->next[lv] &= UINTPTR_MAX-1;
				hopelesness = 0;
				acceptance++;
				if(acceptance >= MAXCHAOS)
				{
					// Unmarking all next pointers and
					// giving up.
					printf("(R %u): I give up.\n", key);
					for(;lv>=0;lv--)
						target->next[lv] &= UINTPTR_MAX-1;
					return -4;
				}
			}
		}
		
		nextNode = getPtr(curNode->next[lv]);
		if(!nextNode)
		{
			chaosmeter++;
			goto slRemoving;
		}
		// Move right until successor is found
		if(nextNode->key < key)
		{
			curNode = nextNode; // do some checks? del
			goto slRemoving;
		}
		
		// Target found
		if(nextNode->key == key)
		{
			if(!started)
			{
				target = nextNode;
				target->stopflag = 1;
				if(lv == slLEVELS-1)
				{
					getPtr(target->next[lv])->previous = curNode;
					removedFromTop = 1;
				}
				started = 1;
			}
			if(atomic_fetch_or(&(target->next[lv]), 1) & 1)
			{
				#if DBG_WARNINGS >= 3
				printf("Remove: bit stealing failed (%u).\n",key);
				#endif
				// Already marked for deletion
				chaosmeter++;
				//continue;
			}
			if(!atomic_compare_exchange_strong(&(curNode->next[lv]),
			   &target, target->next[lv] & (UINTPTR_MAX ^ 1)))
			{
				// Don't want misaligned local pointer
				target = (slNode *)((atomic_uintptr_t)target&(UINTPTR_MAX ^ 1));
				#if DBG_WARNINGS >= 3
				printf("Remove: CAS failed (curnxt:%u,nxt:%u, k:%u).\n"
				       , curNode->next[lv],nextNode,key);
				#endif
				// CAS failed. Reset flag and start over.
				//if(nextNode->next[lv]) // dbg, prob useless
				//target->next[lv] &= (UINTPTR_MAX ^ 1);
				chaosmeter++;
				goto slRemoving;
			}
			curNode->next[lv] &= (UINTPTR_MAX ^ 1);
			
		}
		if(lv)	lv--;
		else if(started)
		{
			free(target->next);
			free(target);
			if(removedFromTop)	return 1;
			return 0;
		}
		else	return -1;
	}
}


// Wouldn't it be nice if this was commented? }:‑)
// Returns a number between 1 and slLEVELS
// Probability that result >= n is about 1/2^n
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
	// TODO remove redundant condition? return results+1 should work as well.
	return (result>=slLEVELS)?slLEVELS:result+1;
}

// A lovely multi-level skiplist printer
void slPrint(slNode * slHead)
{
	slNode * tmpNode;
	for(int8_t lv = slLEVELS-1; lv >= 0; lv--)
	{
		printf("Level %u: ", lv);
		tmpNode = slHead;
		while(tmpNode->key != MAXKEY)
		{
			printf("%u, ", tmpNode->key);
			tmpNode = getPtr(tmpNode->next[lv]);
		}
		printf("%u\n", tmpNode->key);
	}
}

// Strip stolen bit, get nude pointer.
slNode * getPtr(atomic_uintptr_t p)	
{
	return (slNode *)(p & (UINTPTR_MAX ^ 1));
}

// Return a node's predecessor on any given level
// Not actually using this for the time being / ever.
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
