#ifndef SKIPLIST_H
#define SKIPLIST_H


#include "skiplist.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/*
"log log u levels" "where u is the size of the key space"
let u = 2^30 = 2147483648 = 1073741824
log log u = 3.0346 ≅ 4
*/
#define slLEVELS 4	//TODO dynamic levels


typedef struct slNode_ {
	int key;
	struct slNode_ ** next;
	// interact with the x-fast trie
	struct slNode_ * previous;
	char removing;

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
char slInsert(slNode * slHead, int newKey);


// return a pointer to the node which contains the desired key, or NULL if 404
slNode * slFind(slNode * slHead, int key);

/*
Remove node whose value matches key
return 0 if successful
return -1 if 404
*/
char slRemove(slNode * slHead, int key);

uint8_t flipcoins();

#endif
