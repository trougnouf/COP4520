#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>

#define slLEVELS 4

typedef struct slNode_ {
	uint32_t key;
	//struct slNode_ ** next;
	atomic_uintptr_t  * next;
	// interact with the x-fast trie
	struct slNode_ * previous;
	uint8_t stopflag;
} slNode;

void main()
{
	uint8_t numLv = 3;
	slNode * newNode = malloc(sizeof(slNode));
	newNode->next = malloc(sizeof(atomic_uintptr_t)*(numLv));
	slNode * anotherNode = malloc(sizeof(slNode));
	anotherNode->next = malloc(sizeof(atomic_uintptr_t)*(numLv));
	newNode->key = 63;
	newNode->next[0] = 
	
	
	uint8_t * results = calloc(slLEVELS, sizeof(uint8_t));
	uint8_t curResult;
	for(int i=0; i<5000; i++)
	{
		curResult = flipcoins();
		results[curResult-1] ++;
	}
	for(int i=0; i<slLEVELS; i++)
	{
		printf("%u: %u\n", i+1, results[i]);
	}
	
	uint8_t j = 8;
	for(;j>=20;j--);
	printf("%u", j);
}
