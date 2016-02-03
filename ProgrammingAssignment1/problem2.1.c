#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMTHREADS 5	// 254 max

#define MAXSLEEP 5

struct chopstick
{
	uint8_t location;
	uint8_t usedBy;	// UINT8_MAX = no one
};

struct philosopherData
{
	uint8_t location;
	uint8_t isThinking;
	struct chopstick * lChopstick;
	struct chopstick * rChopstick;
};

void* thread_philosopher(void* args)
{
	setbuf(stdout, NULL);
	srand(time(NULL));
	struct philosopherData * data = &(*(struct philosopherData *)args);
	printf("%u is now thinking.\n", (*data).location);
	
	for(;;)
	{
		if((*data).location == UINT8_MAX)	break;
		
		// thinking
		if((*data).isThinking)
		{
			sleep(rand() % MAXSLEEP + 1);
			(*data).isThinking = 0;	// switch to hungry
			printf("%u is now hungry.\n", (*data).location);
		}
		// eating
		else if(((*(*data).lChopstick).usedBy == (*data).location) &&
			((*(*data).rChopstick).usedBy == (*data).location))
		{
			printf("%u is now eating.\n", (*data).location);
			sleep(rand() % MAXSLEEP + 1);
			printf("%u is now thinking.\n", (*data).location);
			(*data).isThinking = 1;	// switch to thinking
			(*(*data).lChopstick).usedBy = UINT8_MAX;
			(*(*data).rChopstick).usedBy = UINT8_MAX;
		}
		// hungry
		else
		{
			if((*(*data).lChopstick).usedBy == UINT8_MAX)
				(*(*data).lChopstick).usedBy = (*data).location;
			if((*(*data).rChopstick).usedBy == UINT8_MAX)
				(*(*data).rChopstick).usedBy = (*data).location;
		}
	}
		
	pthread_exit(NULL);
}

int main()
{
	// initialize chopsticks and philosophers
	struct chopstick chopsticks[NUMTHREADS];
	for(uint8_t i = 0; i < NUMTHREADS; i++)
	{
		chopsticks[i].location = i;
		chopsticks[i].usedBy = UINT8_MAX;
	}
	struct philosopherData philosophersD[NUMTHREADS];
	pthread_t philosophersT[NUMTHREADS];
	for(uint8_t i = 0; i < NUMTHREADS; i++)
	{
		philosophersD[i].location = i;
		philosophersD[i].isThinking = 1;
		philosophersD[i].lChopstick = &chopsticks[i];
		philosophersD[i].rChopstick = &chopsticks[i==(NUMTHREADS-1)?0:i+1];
		pthread_create(&philosophersT[i], NULL, thread_philosopher, (void *)&philosophersD[i]);
	}
	
	// 'n' + enter = end
	char terminator;
	while(terminator != 'n')	terminator = getchar();
	for(uint8_t i = 0; i < NUMTHREADS; i++)
		philosophersD[i].location = UINT8_MAX;
	
	// wait for threads to end
	for(uint8_t i=0; i<NUMTHREADS; i++)
	{
		pthread_join(philosophersT[i], NULL);
	}
}
