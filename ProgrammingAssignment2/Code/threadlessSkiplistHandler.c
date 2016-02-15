#include "skiplist.h"
#include <sys/time.h>

#define NUMKEYS 500000
#define VERBOSE 0

double getTimeElapsed(struct timeval * begTime);

int main()
{
	char nextTask;
	int testKeys[NUMKEYS];
	for(int i=0; i < NUMKEYS; i++)
	{
		testKeys[i] = rand() % 1073741824;
	}
	// manually change keys here
	testKeys[1] = 23;
	testKeys[23] = 23;
	
	// start timer
	struct timeval begTime;
	gettimeofday(&begTime, NULL);

	// init Data Structures
	slNode * slHead = slInit();
	
	//char returnFlag;
	setbuf(stdout, NULL);
	for(int i=0; i<NUMKEYS; i++)
	{
		nextTask = rand() % 2;
		if(nextTask)	printf("%d: %d\n", testKeys[i], slInsert(slHead, testKeys[i]));
		else
		{	printf("d%d: ",testKeys[i]);
			printf("%d\n", slRemove(slHead, testKeys[i]));
		}
	
	
		if(i%1000==0)	printf("%d\n", i);
		
		//returnFlag = slInsert(slHead, testKeys[i]);
		slInsert(slHead, testKeys[i]);
		//printf("%d: %d\n", testKeys[i], slInsert(slHead, testKeys[i]));
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
