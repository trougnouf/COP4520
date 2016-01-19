#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <math.h>
#define MAXNUM 100000001	// Pick an odd number and add 1
#define MAXTHREADS 7		// Not including the main function
#define TESTING 0		// 0=Run, 1=Testing, 2=Testing+ExtendedRuntime

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
*/

void* thread_compositeFinder(void* args);	// thread

uint8_t * isComposite;				// global array

						// function prototypes
uint32_t nextPrime(uint32_t * curPrime, uint32_t * searchLimit);		
void printResults(struct timeval * begTime);
double getTimeElapsed(struct timeval * begTime);
void printTime();

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
	isComposite[1] = 1;
	pthread_t threads[MAXTHREADS];
	struct fiveBytes threadData[MAXTHREADS];
	uint32_t searchLimit = sqrt(MAXNUM);
	for(uint8_t i=0; i<MAXTHREADS; i++)
	{
		// workaround required for initialization w/optimizations
		curPrime = (i==0|i==1)?i+2:nextPrime(&curPrime, &searchLimit);
		threadData[i].curValue = curPrime;
		threadData[i].isWaiting = 0;
		pthread_create(	&threads[i], NULL, thread_compositeFinder,
				(void *) &threadData[i]);
	}	
		// workaround for single thread execution w/optimizations
	curPrime = (curPrime == 2)?3:nextPrime(&curPrime, &searchLimit);
	
	// loop over each thread, give out work when necessary
	while(curPrime)
	{
		for(uint8_t i = 0; i < MAXTHREADS; i++)
		{
			if(threadData[i].isWaiting)
			{
				threadData[i].curValue = curPrime;
				threadData[i].isWaiting = 0;
				curPrime = nextPrime(&curPrime, &searchLimit);
			}
		}
	}
	
	// make all threads commit suicide
	for(uint8_t i = 0; i < MAXTHREADS; i++)
	{
		threadData[i].curValue = 0;
		
	}
	for(uint8_t i = 0; i < MAXTHREADS; i++)	pthread_join(threads[i], NULL);
	
	printResults(&begTime);

	/*
	// uncomment this to get the complete list of prime numbers
	for(uint32_t i=0; i<MAXNUM; i++)
		printf("%u:\t%u\n", i, isComposite[i]);
	*/
}


// precise function
void* thread_compositeFinder(void* args)
{
	uint32_t cur;
	uint8_t isWaiting;
	for(;;)
	{
		
		isWaiting = (*(struct fiveBytes*)args).isWaiting;
		cur = (*(struct fiveBytes*)args).curValue;
		
		if(!cur)
		{
			pthread_exit(NULL);
			return NULL;
		}
		if(!isWaiting)
		{
			for(uint32_t i=cur*cur; i<MAXNUM; i+=cur*2)
				isComposite[i] = 1;
			(*(struct fiveBytes*)args).isWaiting = 1;
			continue;
		}
	}
}

uint32_t nextPrime(uint32_t * curPrime, uint32_t * searchLimit)
{
	if(!(*curPrime))	return 0;
	for(uint32_t i = *curPrime+2; *curPrime < *searchLimit; i+=2)
		if(!isComposite[i])	return i;
	return 0;
}

void printResults(struct timeval * begTime)
{
	uint32_t numOfPrimes = 0;
	uint64_t sumOfPrimes = 0;
	uint32_t maxPrimes[10];
	
	// it takes approximately 0.30s to analyze the results on a single thread
	if(TESTING == 2)
		printf(	"Execution time: %fs (excluding print statement)\n",
		getTimeElapsed(begTime));
	
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
	if(!TESTING)
	{
		printf(	"Execution time: %fs\n",
			getTimeElapsed(begTime));
		printf("Found %u primes.\n", numOfPrimes);
		printf("Sum of primes = %u\n", sumOfPrimes);
		printf("Top ten maximum primes: ");
		for(int8_t i = 9; i >= 0; i--)
			printf("%u%s", maxPrimes[i], i?", ":"\n");
	}
	else
	{
		printTime();
		printf(	
"\tThr= %u\tNum= %u\tRes= %u\tPerf= %f\n", MAXTHREADS,
			MAXNUM, numOfPrimes, getTimeElapsed(begTime));
	}
}


double getTimeElapsed(struct timeval * begTime)
{
	struct timeval curTime;
	gettimeofday(&curTime, NULL);
	return (double)	((curTime.tv_usec - (*begTime).tv_usec)/1000000. +
			curTime.tv_sec - (*begTime).tv_sec);
}

/*
Source: Hamid Nazari
http://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
Used for testing only. (TESTING != 0)
*/
void printTime()	
{
	time_t timer;
	char buffer[26];
	struct tm* tm_info;

	time(&timer);
	tm_info = localtime(&timer);

	strftime(buffer, 26, "%Y:%m:%d %H:%M:%S", tm_info);
	printf(buffer);
}
