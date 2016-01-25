#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
//#include <math.h>

#define NUMTHREADS 5
#define NUMCSTICKS 5

#define MINDELAY 1
#define MAXDELAY 60

struct chopstick
{
	uint8_t location;
	uint8_t isBusy;
};

struct philosopherData
{
	uint8_t location;
	uint8_t isThinking;
	struct chopstick * chopsticks;
};

void* thread_philosopher(void* args)
{
	uint8_t location = (*(struct philosopherData *)args).location;
	double nextSwitch;

	//printf(" %u ", location);
	struct philosopherData * data = &(*(struct philosopherData *)args);
	//printf(" %u ", (*data).isThinking);
	
	for(;;)
	{
		if((*data).isThinking)	// thinking
		{
			if(isTimeToSwitch(nextSwitch))
			{
				printf("%u is now hungry.\n", location);
				(*data).isThinking = 0; // switch to hungry
			}
		}
		else					// hungry or eating
		{
			if(hasChopsticks(data) == 2)	// eating
			{
				if(isTimeToSwitch(nextSwitch))	// full
				{
					printf("%u is now thinking.\n", location);
					(*data).isThinking = 1; // switch to thinking
					releaseSticks(data);
					nextSwitch = getNextSwitch();
				}
			}
			else	// hungry
			{
				getChopsticks(data, hasChopsticks(data)); // switch to eating
				printf("%u is now eating.\n", location);
				nextSwitch = getNextSwitch();
			}
		}
	}
	//static uint8_t location;
	//uint8_t isThinking;
	//struct chopstick * chopsticks[NUMCSTICKS];
	pthread_exit(NULL);
}

int main()
{
	// initialize chopsticks and philosophers
	struct chopstick chopsticks[NUMCSTICKS];
	for(uint8_t i = 0; i < NUMCSTICKS; i++)
	{
		chopsticks[i].location = i;
		chopsticks[i].isBusy = 0;
	}
	struct philosopherData philosophersD[NUMTHREADS];
	pthread_t philosophersT[NUMTHREADS];
	for(uint8_t i = 0; i < NUMTHREADS; i++)
	{
		philosophersD[i].location = i;
		philosophersD[i].isThinking = 0;
		philosophersD[i].chopsticks = chopsticks;
		pthread_create(&philosophersT[i], NULL, thread_philosopher, (void *)&philosophersD[i]);
	}
	
	
	// wait for threads to end
	for(uint8_t i=0; i<NUMTHREADS; i++)
	{
		pthread_join(philosophersT[i], NULL);
	}


}

// compare time to switch to current time
uint8_t isTimeToSwitch(double targetT)
{
	struct timeval curTime;
	gettimeofday(&curTime, NULL);
	if(curTime.tv_sec > targetT)	return 1;
	return 0;
}

double getNextSwitch()
{
	struct timeval curTime;
	gettimeofday(&curTime, NULL);
	return curTime.tv_sec + 5;	// TODO: change to  + random number (not specified in instructions)
}

uint8_t hasChopsticks(struct philosopherData * phiDat)
{
	struct chopstick * chopsticks = (*phiDat).chopsticks;
	uint8_t numSticks = 0;
	for(uint8_t i=0; i<NUMCSTICKS; i++)
	{
		if(chopsticks[i].location == (*phiDat).location)	numSticks++;
	}
	return numSticks;
}

void getChopsticks(struct philosopherData * phiDat, uint8_t curChopsticks)
{
	struct chopstick * chopsticks = (*phiDat).chopsticks;
	uint8_t chopsticksNeeded = curChopsticks?1:2;
	while(chopsticksNeeded)
	{
		uint8_t closestSeat = (*phiDat).location;
		for (uint8_t i = 1; i<=NUMTHREADS; i++)
		{
			closestSeat = getClosestSeat(closestSeat, i);
			if(!chopsticks[closestSeat].isBusy)
			{
				chopsticks[closestSeat].isBusy = 1;
				chopsticks[closestSeat].location = (*phiDat).location;
				chopsticksNeeded--;
				break;
			}
		}	
	}
}

uint8_t getClosestSeat(uint8_t curSeat, uint8_t i)
{
	int8_t result = curSeat + ((i%2)?-1:1)*i;
	if(result >= NUMTHREADS)	result -= NUMTHREADS;
	else if(result < 0)			result += NUMTHREADS;
	return (uint8_t) result;
}

void releaseSticks(struct philosopherData * phiDat)
{
	struct chopstick * chopsticks = (*phiDat).chopsticks;
	for(uint8_t i = 0; i < NUMCSTICKS; i++)
	{
		if(chopsticks[i].location == (*phiDat).location)
		{
			chopsticks[i].isBusy = 0;
		}
	}
}
