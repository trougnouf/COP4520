/*
Sandbox
*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

typedef enum {FIND, INSERT, REMOVE, IDLE, TERM} state;

typedef struct
{
	state curState;
	int ioData;
	
} pthreadData;

void* thread_dsHandler(void* args)
{
	pthreadData * myData = (pthreadData *)args;
	int data = myData->ioData;
	int data2 = ((pthreadData *)args)->ioData;
	state curTask = ((pthreadData *)args)->curState;
	printf("current task: %d\n", curTask);
	pthread_exit(NULL);
	for(;;)
	{
		
	}
}

int main()
{
	pthread_t threads[1];
	pthreadData threadData[1];
	threadData[1].curState = IDLE;
	threadData[1].ioData = 8;
	pthread_create(&threads[1], NULL, thread_dsHandler, (void *) &threadData[1]);
	sleep(1);
	pthread_join(threads[1], NULL);
}
