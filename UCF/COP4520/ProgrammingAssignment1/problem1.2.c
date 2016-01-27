#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <math.h>
#define MAXNUM 100000001	// Pick an odd number and add 1
#define MAXTHREADS 8		// Not including the main function. Fastest when MAXTHREADS = CPU threads - 1
#define TESTING 0

/*
This program computes the prime numbers located between 2 and MAXNUM using 
MAXTHREADS threads. The last ten prime numbers are display, but the complete
list can be sent to STDOUT by uncommenting the end of the main function

Run as follow: 
$ gcc -lpthread -lm problem1.c && ./a.out >> primes.txt
or with compiler optimization:
$ gcc -lpthread -lm -march=native -O2 problem1.c && ./a.out >> primes.txt
or in failsafe mode:
$ gcc -lpthread -lm -std=gnu11 problem1.c && ./a.out >> primes.txt

This version always returns the right results but it runs much slower.
*/


void* thread_compositeFinder(void* args);	// thread

uint32_t findNextPrime(volatile uint32_t * curPrime, uint32_t * searchLimit, volatile uint8_t * isComposite);
void printResults(struct timeval * begTime, volatile uint8_t * isComposite);
double getTimeElapsed(struct timeval * begTime);

pthread_mutex_t nextPrimeMutex;

struct cF_data
{
	volatile uint32_t * highestPrime;
	uint32_t * searchLimit;
	volatile uint8_t * isComposite;
	volatile uint8_t * getNextPrime;
};

void* thread_compositeFinder(void* args)
{
	
	uint32_t curPrime;
	for(;;)
	{
		//pthread_mutex_lock(&nextPrimeMutex);
		//*(*(struct cF_data*)args).highestPrime = findNextPrime(
		//	(*(struct cF_data*)args).highestPrime,
		//	(*(struct cF_data*)args).searchLimit,
		//	(*(struct cF_data*)args).isComposite);
		curPrime = *(*(struct cF_data*)args).highestPrime;
		*(*(struct cF_data*)args).getNextPrime = 1;
		//pthread_mutex_unlock(&nextPrimeMutex);
		
		if(curPrime == 0)	break;
		for(uint32_t i=curPrime*curPrime; i<MAXNUM; i+=curPrime*2)
			(*(struct cF_data*)args).isComposite[i] = 1;
	}
	pthread_exit(NULL);
	
}

void* thread_nextPrimeFinder(void* args)
{
	for(;;)
	{
		
		if(*(*(struct cF_data*)args).getNextPrime)
		{
			*(*(struct cF_data*)args).highestPrime = findNextPrime(
				(*(struct cF_data*)args).highestPrime,
				(*(struct cF_data*)args).searchLimit,
				(*(struct cF_data*)args).isComposite);
			*(*(struct cF_data*)args).getNextPrime = 0;
			if(*(*(struct cF_data*)args).highestPrime == 0)
				pthread_exit(NULL);
		}
	}
}

int main()
{
	
	uint32_t searchLimit = sqrt(MAXNUM);
	struct timeval begTime;
	gettimeofday(&begTime, NULL);
	volatile uint8_t * isComposite;
	isComposite = (uint8_t*) calloc(MAXNUM, 1);
	isComposite[0] = 1;
	isComposite[1] = 1;
	volatile uint8_t getNextPrime = 1;
	pthread_t threads[MAXTHREADS];
	struct cF_data threadData[MAXTHREADS];
	volatile uint32_t highestPrime = 1;
	pthread_mutex_init(&nextPrimeMutex, NULL);
	for(uint8_t i=0; i<MAXTHREADS; i++)
	{
		threadData[i].highestPrime = &highestPrime;
		threadData[i].searchLimit = &searchLimit;
		threadData[i].isComposite = isComposite;
		threadData[i].getNextPrime = &getNextPrime;
		if(i)
			pthread_create(	&threads[i], NULL, thread_compositeFinder,
				(void *) &threadData[i]);
		else
			pthread_create(	&threads[i], NULL, thread_nextPrimeFinder,
				(void *) &threadData[i]);
	}
	for(uint8_t i = 0; i < MAXTHREADS; i++)	pthread_join(threads[i], NULL);
	printResults(&begTime, isComposite);
	
}

uint32_t findNextPrime(volatile uint32_t * curPrime, uint32_t * searchLimit, volatile uint8_t * isComposite)
{
	if(*curPrime < 3)
	{
		switch (*curPrime)
		{
			case 1: return 2;
			case 2: return 3;
		}
	}
	for(uint32_t i = *curPrime+2; i < *searchLimit; i+=2)
		if(!isComposite[i])	return i;
	return 0;
}

void printResults(struct timeval * begTime, volatile uint8_t * isComposite)
{
	uint32_t numOfPrimes = 0;
	uint64_t sumOfPrimes = 0;
	uint32_t maxPrimes[10];
	
	for(uint32_t i = MAXNUM-2; i > 1; i-=2)
	{
		if(!isComposite[i])
			{
				if(numOfPrimes < 10)
					maxPrimes[numOfPrimes] = i;
				numOfPrimes++;
				sumOfPrimes += i;
			}
	}
	sumOfPrimes +=2;
	numOfPrimes++;

	printf(	"Execution time: %fs\n",
		getTimeElapsed(begTime));
	printf("Found %u primes.\n", numOfPrimes);
	printf("Sum of primes = %lu\n", sumOfPrimes);
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
