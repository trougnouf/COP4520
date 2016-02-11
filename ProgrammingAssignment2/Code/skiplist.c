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
	find previous node and next node that have a higher level
*/
	
	slNode * curNode = slHead;
	slNode * closestNode[slLEVELS];
	char lv;
	for(lv = slLEVELS-1; lv > 0; lv--)
	{
		while((*curNode).key < newKey)	// if curNode is smaller
		{
			if(curNode->next[lv] && (*curNode->next[lv]).key<newKey)
			{
				curNode = curNode->next[lv];
			}
			else	// keep track of closestNode and go down
			{
				closestNode[lv] = curNode;
				break;
			}
		}
	}
	for(;;)	// bottom reached; create and add node
	{
		if(!(curNode->next[lv]) || (*curNode->next[lv]).key>newKey)
		{ // become next node
			closestNode[lv] = curNode;
			int coin = rand() % (slLEVELS*2)+1;
			slNode * newNode = malloc(sizeof(slNode) +
					   sizeof(slNode*)*((slLEVELS*2)/coin));
			(*newNode).key = newKey;
			for(lv = 0; lv < slLEVELS; lv++)
			{

				if((coin) <= (slLEVELS*2)/(lv+1))
				{
					newNode->next[lv] = closestNode[lv]->next[lv];
					closestNode[lv]->next[lv] = newNode;
					if(lv == slLEVELS-1)	return 1;
				}
				else return 0;
			}
			slNode * tmpNode = curNode->next[lv];
			break;
		}
		else curNode = curNode->next[lv];
	}
	
	
}

int main()
{
	slInit();
	slInsert(50);
	slInsert(51);
	slInsert(52);
}


