#ifndef SKIPLIST_H
#define SKIPLIST_H


#include "skiplist.h"
#include "testcase1.h"	// Removable

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>

/*
"log log u levels" "where u is the size of the key space"
let u = 2^30 = 2147483648 = 1073741824
log log u = 3.0346 ≅ 4
*/
#ifndef slLEVELS
#define slLEVELS 4	// 1-128 (sane values: 4-16) TODO dynamic levels
#define MAXCHAOS 50
#define slMERGEDUPLICATES 0	// 0: detect duplicates prior to insertion
				// 1: start inserting right away, merge
				//    duplicates if they come up after the 
				//    insertion has begun. Slower insertion, 
				//    faster retrieval of frequently used nodes.
				//    (unstable)
#endif
typedef struct slNode_ {
	uint32_t key;
	//struct slNode_ ** next;
	atomic_uintptr_t  * next;
	// interact with the x-fast trie
	struct slNode_ * previous;
	uint8_t stopflag;
} slNode;


/*
initialize skiplist with head.key=INT_MIN and tail=NULL
*/
slNode * slInit();


/* Insert key into skiplist.
return -1: newKey exists
return 0:  sucess, nothing left to do
return 1:  sucess, top-level reached (x-fast trie required)
*/
slNode * slInsert(slNode * slHead, uint32_t key);


// return a pointer to the node which contains the desired key, or NULL if 404
slNode * slFind(slNode * slHead, uint32_t key);

/*
Remove node whose value matches key
return 0 if successful
return -1 if 404
*/
int8_t slRemove(slNode * slHead, uint32_t key);

// Helper functions:

/*
	We use this function to find the direct predecessor of any given node 
	when we a next node is marked for deletion
*/
slNode * findPredecessor(slNode ** predecessors, uint8_t lv, uint32_t value);

uint8_t flipcoins();

slNode * getPtr(atomic_uintptr_t p);

slNode * slMerge(slNode * oldNode, slNode * newNode, slNode * curNode, int8_t oldLv);

void slPrint(slNode * slHead);

#endif
