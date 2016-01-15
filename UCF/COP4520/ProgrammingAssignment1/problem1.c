#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#define MAXNUM 100000001
#define MAXTHREADS 8

void* thread_compositeFinder(void* args);
uint8_t * isComposite;
uint32_t nextPrime(uint32_t curPrime);

struct fiveBytes {
	volatile uint32_t curValue;
	volatile uint32_t isWaiting;
};

int main()
{
	uint32_t curPrime = 1;
	// initialize empty array of 10^8 characters, threads, and their data
	isComposite = (uint8_t*) calloc(MAXNUM, 1);
	isComposite[0] = 1;
	pthread_t threads[MAXTHREADS];
	volatile struct fiveBytes threadData[MAXTHREADS];
	for(uint8_t i=0; i<MAXTHREADS; i++)
	{
		curPrime = nextPrime(curPrime);
		threadData[i].isWaiting = 0;
		threadData[i].curValue = curPrime;
		pthread_create(	&threads[i], NULL, thread_compositeFinder,
				(void *) &threadData[i]);
	}	
	curPrime = nextPrime(curPrime);
	// loop over each thread, give out work when necessary
	while(curPrime)
	{
		for(uint8_t i = 0; i < MAXTHREADS; i++)
		{
			if(threadData[i].isWaiting)
			{
				threadData[i].curValue = curPrime;
				threadData[i].isWaiting = 0;
				curPrime = nextPrime(curPrime);
			}
		}
	}
	
	// make all threads commit suicide
	for(uint8_t i = 0; i < MAXTHREADS; i++)
	{
		threadData[i].curValue = 0;
		pthread_join(threads[i], NULL);
	}

	for(uint32_t i=0; i<MAXNUM; i++)	printf("%u:\t%u\n", i, isComposite[i]);
}

void* thread_compositeFinder(void* args)
{
	uint32_t cur;
	for(;;)
	{
		cur = (*(struct fiveBytes*)args).curValue;
		if(!cur)
		{
			pthread_exit(NULL);
			return NULL;
		}
		else if(!(*(struct fiveBytes*)args).isWaiting)
		{
			for(uint32_t i=cur*2; i<MAXNUM; i+=cur)
				isComposite[i] = 1;
			(*(struct fiveBytes*)args).isWaiting = 1;
		}
	}
}

uint32_t nextPrime(uint32_t curPrime)
{
	if(!curPrime)	return 0;
	for(uint32_t i = curPrime+1; curPrime < MAXNUM; i++)
		if(!isComposite[i])	return i;
	return 0;
}

/*
 * Benchmark (on Thinkpad X200t old laptop):
 * Single thread, no optimization: 6976B, 44s (~2% dif w/"dynamic programming", 
 */
