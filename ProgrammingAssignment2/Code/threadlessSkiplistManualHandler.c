#include "skiplist.h"
#include <sys/time.h>

#define NUMKEYS 1000
#define VERBOSE 0

double getTimeElapsed(struct timeval * begTime);

int main()
{
	int testKeys[NUMKEYS];
	for(int i=0; i < NUMKEYS; i++)
	{
		testKeys[i] = rand() % 1073741824;
	}
	// manually change keys here
	testKeys[1] = 23;
	testKeys[23] = 23;
	testKeys[100] = 5;
	testKeys[155] = 6;
	testKeys[150] = 5;
	testKeys[1000] = 9;
	
	// start timer
	struct timeval begTime;
	gettimeofday(&begTime, NULL);

	// init Data Structures
	slNode * slHead = slInit();
	
	//char returnFlag;
	
	for(int i=0; i<NUMKEYS; i++)
	{
	
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
