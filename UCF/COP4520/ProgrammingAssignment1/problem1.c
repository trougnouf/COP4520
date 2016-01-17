#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#define MAXNUM 100000001
#define MAXTHREADS 1

void* thread_compositeFinder(void* args);	// thread

volatile uint8_t * isComposite;				// global array

						// function prototypes
uint32_t nextPrime(uint32_t * curPrime);		
void printResults(struct timeval * begTime);
double getTimeElapsed(struct timeval * begTime);

struct fiveBytes {
	volatile uint32_t curValue;
	volatile uint32_t isWaiting;
};

int main()
{
	struct timeval begTime;
	gettimeofday(&begTime, NULL);
	uint32_t curPrime = 1;
	// initialize empty array of 10^8 characters, threads, and their data
	isComposite = (uint8_t*) calloc(MAXNUM, 1);
	isComposite[0] = 1;
	pthread_t threads[MAXTHREADS];
	volatile struct fiveBytes threadData[MAXTHREADS];
	for(uint8_t i=0; i<MAXTHREADS; i++)
	{
		curPrime = nextPrime(&curPrime);
		threadData[i].isWaiting = 0;
		threadData[i].curValue = curPrime;
		pthread_create(	&threads[i], NULL, thread_compositeFinder,
				(void *) &threadData[i]);
	}	
	curPrime = nextPrime(&curPrime);
	// loop over each thread, give out work when necessary
	while(curPrime)
	{
		for(uint8_t i = 0; i < MAXTHREADS; i++)
		{
			if(threadData[i].isWaiting)
			{
				threadData[i].curValue = curPrime;
				threadData[i].isWaiting = 0;
				curPrime = nextPrime(&curPrime);
			}
		}
	}
	
	// make all threads commit suicide
	for(uint8_t i = 0; i < MAXTHREADS; i++)
	{
		threadData[i].curValue = 0;
		pthread_join(threads[i], NULL);
	}
	
	printResults(&begTime);

	/*
	// uncomment this to get the complete list of prime numbers
	for(uint32_t i=0; i<MAXNUM; i++)
		printf("%u:\t%u\n", i, isComposite[i]);
	*/
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

uint32_t nextPrime(uint32_t * curPrime)
{
	if(!(*curPrime))	return 0;
	for(uint32_t i = *curPrime+1; *curPrime < MAXNUM; i++)
		if(!isComposite[i])	return i;
	return 0;
}

void printResults(struct timeval * begTime)
{
	uint32_t numOfPrimes = 0;
	uint64_t sumOfPrimes = 0;
	uint32_t maxPrimes[10];
	/*
	// it takes approximately 0.30s to analyze the results on a single thread
	printf(	"Execution time: %fs (excluding print statement)\n",
		getTimeElapsed(begTime));
	*/
	for(uint32_t i = MAXNUM-1; i > 1; i--)
	{
		if(!isComposite[i])
			{
				if(numOfPrimes < 10)
					maxPrimes[numOfPrimes] = i;
				numOfPrimes++;
				sumOfPrimes += i;
			}
	}
	printf(	"Execution time: %fs\n",
		getTimeElapsed(begTime));
	printf("Found %u primes.\n", numOfPrimes);
	printf("Sum of primes = %u\n", sumOfPrimes);
	printf("Top ten maximum primes: ");
	for(int8_t i = 9; i >= 0; i--)
		printf("%u%s", maxPrimes[i], i?", ":"\n");
}

double getTimeElapsed(struct timeval * begTime)
{
	struct timeval curTime;
	gettimeofday(&curTime, NULL);
	return (double)	((curTime.tv_usec - (*begTime).tv_usec)/1000000. +
			curTime.tv_sec - (*begTime).tv_sec);
}
