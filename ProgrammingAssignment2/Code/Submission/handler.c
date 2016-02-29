/*
This file contains anything that isn't specific to one particular data structure
or test case.
*/

/*
TODO: Add x-fast trie functions to switch() statements
*/
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdatomic.h>

#include "handler.h"
#include "testcase1.h"
#include "skiplist.h"


void* thread_dsHandler(void* args)
{
	#if VERBOSE == 1
	setbuf(stdout, NULL);
	#endif
	pthreadData * sharedData = (pthreadData *)args;
	Task * todolist = sharedData->todolist;
	char retStat;
	int tsk;	// counter
	for(;;)
	{

		// get global position from shared todolist and increment
		tsk = atomic_fetch_add(sharedData->tsknum, 1);
		
		#if VERBOSE == 1
			printf("thread %d processing task#\t%d (%d, %d)\n", 
				sharedData, tsk, todolist[tsk].task,
				todolist[tsk].inData);
		#endif
		#if VERBOSE > 500
			if(tsk%VERBOSE == 0)	printf("Task %d\n", tsk);
		#endif
		
		
		switch(todolist[tsk].task)
		{
			// define thread behavior here (and/or call functions)
			case FIND:
				// 1=found, 0=404
				retStat =	(slFind(sharedData->slHead,
						todolist[tsk].inData)!=NULL);
				#if VERBOSE == 3
				printf(	"Found %d in skiplist: %d\n",
					todolist[tsk].inData, retStat);
				#endif
				//TODO xfind()
				break;
			case INSERT:
				// -1=already exists, 0=success(done), 1=sucess
				//     (top-level reached, x-fast trie required)
				retStat =	slInsert(sharedData->slHead,
						todolist[tsk].inData);
				#if VERBOSE == 3
				printf(	"Added %d to skiplist: %d\n",
					todolist[tsk].inData, retStat);
				#endif
				if(retStat == 1)
					;//TODO call xInsert()
				//curTask = IDLE;
				break;
			case REMOVE:
				// 0=success, -1=404
				retStat =	slRemove(sharedData->slHead,
						todolist[tsk].inData);
				//...
				#if VERBOSE == 3
				printf(	"Removed %d from skiplist: %d\n",
					todolist[tsk].inData, retStat);
				#endif
				//todolist[tsk].inData = IDLE;
				break;
			case TERM:
				#if VERBOSE == 3
				printf(	"Thread: %d: Goodbye.\n",
				sharedData);
				#endif
				pthread_exit(NULL);
				break;
			default:
				break;
		}
	}
}

int main()
{
	// load test case
	Task * todolist = loadTestCase();
	atomic_int curTaskNum = 0;
	
	// start timer
	struct timeval begTime;
	gettimeofday(&begTime, NULL);

	// init Data Structures
	slNode * slHead = slInit();
	
	// init Threads
	pthread_t threads[NUMTHREADS];
	pthreadData threadData[NUMTHREADS];
	for(uint8_t i=0; i<NUMTHREADS; i++)
	{
		threadData[i].slHead = slHead;
		threadData[i].todolist = todolist;
		threadData[i].tsknum = &curTaskNum;
		pthread_create(	&threads[i], NULL, thread_dsHandler,
				(void *) &threadData[i]);
	}
	
	// terminate all threads
	for(uint8_t thr=0; thr<NUMTHREADS; thr++)
	{
		//while(threadData[thr].curState != IDLE);
		//threadData[thr].curState = TERM;
		pthread_join(threads[thr], NULL);
	}
	printResults(&begTime);
}

void printResults(struct timeval * begTime)
{
	printf("The end.\n\tThreads: %d, tasks: %d\n", NUMTHREADS, NUMTASKS);
	printf(	"Tasks breakdown (%%):\n\tinsert: %d, find: %d, remove: %d\n",
		PERCENTINSERT, PERCENTFIND, PERCENTREMOVE);
	printf("Skip list: %d levels.\n", slLEVELS);
	printf("Time elapsed:\n\t%lfs\n", getTimeElapsed(begTime));
}

double getTimeElapsed(struct timeval * begTime)
{
	struct timeval curTime;
	gettimeofday(&curTime, NULL);
	return (double)	((curTime.tv_usec - (*begTime).tv_usec)/1000000. +
			curTime.tv_sec - (*begTime).tv_sec);
}
