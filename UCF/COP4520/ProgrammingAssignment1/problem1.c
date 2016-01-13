//#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#define MAXNUM 100000001

int main()
{
	// initialize empty array of 10^8 characters
	uint8_t * isComposite = (uint8_t*) calloc(MAXNUM, 1);
	isComposite[0] = 1;
	
	for(uint32_t i=2; i<MAXNUM; i++)
	{
		if(!isComposite[i])
		{
			for(uint32_t j=i*2; j<MAXNUM; j+=i)	isComposite[j] = 1;
		}
	}

	for(uint32_t i=0; i<MAXNUM; i++)
		printf("%u:\t%u\n", isComposite[i]);
}
