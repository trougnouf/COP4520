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
#include <stdio.h>

#include "handler.h"
#include "testcase1.h"
#include "skiplist.h"
#include "xtrie.h"


void* thread_dsHandler(void* args)
{
	#if VERBOSE == 1
	setbuf(stdout, NULL);
	#endif
	pthreadData * sharedData = (pthreadData *)args;
	Task * todolist = sharedData->todolist;
	int8_t retStat;
	atomic_uint tsk;	// counter
	slNode * tmpSLNode;
	level_hash *LSS;
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
			/*
				Find closest node in the xtrie, pass it on to
				the skiplist.
			*/
				// TODO tmpSLNode = xfind function()
				// then replace (slFind(sharedData->slHead, w/(slFind(tmpSLNode,)
				retStat =	(slFind(sharedData->slHead,
						todolist[tsk].inData)!=NULL);
				#if VERBOSE == 3
				printf(	"Found %d in skiplist: %d\n",
					todolist[tsk].inData, retStat);
				#endif
				break;
			case INSERT:
			/*
				slInsert() returns the new node if it reached
				the top, in which case xInsert() is called
			*/
				#if VERBOSE == 3
				printf("Adding %u\t", todolist[tsk].inData);
				fflush(stdout);
				#endif
				tmpSLNode = slInsert(sharedData->slHead,
						todolist[tsk].inData);
				#if VERBOSE == 3
				printf(	"Added %d to skiplist: %u\n",
					todolist[tsk].inData, tmpSLNode?tmpSLNode->key:0);
				#endif
				if(tmpSLNode){

					insert_x_trie(tmpSLNode->key,sharedData->LSS, tmpSLNode);
				}
				break;
			case REMOVE:
			/*
				Remove from xtrie, return closest skiplist node,
				remove from skiplist
			*/
				// TODO tmpSLNode = xremove function()
				// then replace (slRemove(sharedData->slHead, w/(slRemove(tmpSLNode,)
				// 0=success, -1=404
				retStat =	slRemove(sharedData->slHead,
						todolist[tsk].inData);
				//...
				#if VERBOSE == 3
				printf(	"Removed %d from skiplist: %d\n",
					todolist[tsk].inData, retStat);
				#endif
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
	atomic_uint curTaskNum = 0;
	
	// start timer
	struct timeval begTime;
	gettimeofday(&begTime, NULL);

	// init Data Structures		

	slNode * slHead = slInit();
	x_node *root = initialize_trie();
	level_hash *LSS = hash_init(root, slHead);

	// init Threads
	pthread_t threads[NUMTHREADS];
	pthreadData threadData[NUMTHREADS];
	for(uint8_t i=0; i<NUMTHREADS; i++)
	{
		threadData[i].slHead = slHead;
		threadData[i].LSS = LSS;
		threadData[i].todolist = todolist;
		threadData[i].tsknum = &curTaskNum;
		pthread_create(	&threads[i], NULL, thread_dsHandler,
				(void *) &threadData[i]);
	}
	
	// terminate all threads
	for(uint8_t thr=0; thr<NUMTHREADS; thr++)
	{
		pthread_join(threads[thr], NULL);
	}
	printResults(&begTime);
	
	#if DBG_PRINTSL == 1
	// Print the skiplist
	slNode * tmpSLNode;
	printf("\n");
	/*for(int8_t lv = slLEVELS-1; lv >= 0; lv--)
	{
		printf("Level %u: ", lv);
		tmpSLNode = slHead;
		while(tmpSLNode->key != MAXKEY)
		{
			printf("%u, ", tmpSLNode->key);
			tmpSLNode = getPtr(tmpSLNode->next[lv]);
		}
		printf("%u\n", tmpSLNode->key);
	}*/
	level_hash *tst = LSS;
	printf("Xtrie key list\n");
	//int loop;
	for(int loop = 0; loop<MAXKEY; loop++){

		if(LSS->lvl[BASE][loop]!=NULL){

			
			printf("%u ", LSS->lvl[BASE][loop]->key);
if(LSS->lvl[BASE][loop]->pointers[BASE] != NULL){
				printf("prev: %u ", LSS->lvl[BASE][loop]->pointers[BASE]->key);
			}
			if(LSS->lvl[BASE][loop]->pointers[RIGHT] != NULL){
				printf("succ: %u ", LSS->lvl[BASE][loop]->pointers[RIGHT]->key);
			}


			printf("\n");

		}
		
	}
	printf("\n");

	#endif
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
