#include "skiplist.h"

//#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

/*
"log log u levels" "where u is the size of the key space"
let u = 2^30 = 2147483648 = 1073741824
log log u = 3.0346 ≅ 4
*/
#define SLLEVELS 4	//TODO dynamic levels


typedef struct slNode_ {
	int key;
	// int data // or use key as data
	struct slNode_ * next[1];
	// may need to keep track of levels here
} slNode;

/*
typedef struct {
} SL;
*/

slNode * slHead;    //	TODO probably shouldn't be a global var

/*
initialize skiplist with head.key=INT_MIN and tail=NULL
*/
void slInit()
{
	slHead = malloc( sizeof(slNode) + sizeof(slNode*)*SLLEVELS );
	for(char i=0; i<SLLEVELS; i++)	slHead->next[i] = NULL;
	// keep track of size in slNode?
}

// Insert key into skiplist. Return 1 if top of skiplist is reached
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
	find previous node and next node that have a higher level (use sizeof?nope)
*/
	
	slNode * curNode = slHead;
	for(char lv = SLLEVELS-1; lv > 0; lv--)
	{
		while((*curNode).key < newKey)
		{
			if(curNode->next[lv] && (*curNode->next[lv]).key<newKey)
			{
				curNode = curNode->next[lv];
			}
			else break;	// go down
		}
	}
	
	//start on the bottom (level=0)
	//for(int lv=0; lv++; curNode->next[lv])
	
}

int main()
{
	slInit();
}


