#include "skiplist.h"

//#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#define LEVELS 4

struct SLnode {
	int value;
	struct SLnode * next;
	struct SLnode * down;
};

void initSkiplist(struct SLnode * tlNode)
{
	struct SLnode * curNode = tlNode;
	for(char i = 0; i < LEVELS; i++)
	{
		(*curNode).value = INT_MIN;
		(*curNode).next = NULL;
		if(i < LEVELS)
		{
			struct SLnode underNode;
			(*curNode).down = &underNode;
			curNode = &underNode;
		}
	}
}

int main()
{
	struct SLnode tlNode; //topleftNode
	initSkiplist(&tlNode);
	(*(tlNode).down).value = 5;
	tlNode.value = 4;

	printf("%d\n", (*(tlNode).down).down);
}


