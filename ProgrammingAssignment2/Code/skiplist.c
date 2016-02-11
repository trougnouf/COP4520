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
#define slLEVELS 4	//TODO dynamic levels


typedef struct slNode_ {
	int key;
	// int data // or use key as data
	struct slNode_ * next[1];
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
			//slNode * tmpNode = curNode->next[lv];
			break;
		}
		else curNode[lv] = curNode[lv]->next[lv];
	}
}

// return a pointer to the node containing the desired key, or NULL if 404
slNode * slFind(int key)
{
	slNode * curNode = slHead;
	char lv = slLEVELS-1;
	for(;;)
	{
		if(!(*curNode).next[lv])
		{
			if(lv)
			{
				lv--;
				continue;
			}
			return NULL;
		}
		if((*(*curNode).next[lv]).key == key)
			return (*curNode).next[lv];
		if((*(*curNode).next[lv]).key<key)
			curNode=(*curNode).next[lv];	// go right
		else if(lv)	lv--;			// go down
		else	return NULL;			// 404
	}
}

int main()
{
	slInit();
	printf("50 %d\n", slInsert(50));
	printf("50 %d\n", slInsert(50));
	printf("48 %d\n", slInsert(48));
	printf("52 %d\n", slInsert(52));
	printf("56 %d\n", slInsert(56));
	printf("70 %d\n", slInsert(70));
	printf("2 %d\n", slInsert(2));
	printf("50 %d\n", slInsert(50));
	printf("1 %d\n", slInsert(1));
	printf("2 %d\n", slInsert(2));
	printf("4 %d\n", slInsert(4));
	printf("3 %d\n", slInsert(3));
	printf("6 %d\n", slInsert(6));
	printf("5 %d\n", slInsert(5));
	printf("10 %d\n", slInsert(10));
	printf("9 %d\n", slInsert(9));
	printf("%d\n", slFind(10));
	printf("%d\n", slFind(11));
}


