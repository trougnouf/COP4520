#include "skiplist.h"
#include <sys/time.h>

#define NUMKEYS 50000
#define VERBOSE 1

double getTimeElapsed(struct timeval * begTime);

int main()
{
	int testKeys[NUMKEYS];
	for(int i=0; i < NUMKEYS; i++)
	{
		testKeys[i] = rand() & 1073741824;
	}
	
	// start timer
	struct timeval begTime;
	gettimeofday(&begTime, NULL);

	// init Data Structures
	slNode * slHead = slInit();
	
	char returnFlag;
	
	for(int i=0; i<NUMKEYS; i++)
	{
		returnFlag = slInsert(slHead, testKeys[i]);
	}
	
	printf("Time elapsed:\n\t%lfs\n", getTimeElapsed(&begTime));
	
}

double getTimeElapsed(struct timeval * begTime)
{
	struct timeval curTime;
	gettimeofday(&curTime, NULL);
	return (double)	((curTime.tv_usec - (*begTime).tv_usec)/1000000. +
			curTime.tv_sec - (*begTime).tv_sec);
}
