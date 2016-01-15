#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#define MAXNUM 10
#define MAXTHREADS 1

void* thread_compositeFinder(void* args);

uint8_t * isComposite;
volatile uint8_t current;

int main()
{
	// initialize empty array of 10^8 characters
	isComposite = (uint8_t*) calloc(MAXNUM, 1);
	isComposite[0] = 1;
	pthread_t threads[MAXTHREADS];
	current = 2;


	for(uint32_t i=2; i<MAXNUM; i++)
	{
		if(!isComposite[i])
		{
			
		}
		
	}

	//for(uint32_t i=0; i<MAXNUM; i++)	printf("%u:\t%u\n", isComposite[i]);
}

void* thread_compositeFinder(void* args)
{
	uint32_t cur = *((uint32_t *) args);
	
	for(uint32_t i=cur*2; i<MAXNUM; i+=cur)	isComposite[i] = 1;
	
	pthread_exit(NULL);

	return NULL;
}

/*
 * Benchmark (on Thinkpad X200t old laptop):
 * Single thread, no optimization: 6976B, 44s (~2% dif w/"dynamic programming", 
 */
