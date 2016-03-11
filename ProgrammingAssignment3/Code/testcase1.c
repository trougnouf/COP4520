#include "testcase1.h"
#include "handler.h"



Task * loadTestCase()
{
	Task * todolist = malloc((NUMTASKS+NUMTHREADS) * sizeof(Task));
	
	State randTask;
	uint8_t randNum;
	uint32_t i;
	for(i=0; i < NUMTASKS; i++)
	{
		// set tasks here
		randNum = rand() % 101;
		if(randNum <= PERCENTINSERT)			randTask=INSERT;
		else if(randNum <= PERCENTINSERT+PERCENTFIND)	randTask=FIND;
		else						randTask=REMOVE;
		todolist[i].task = randTask;
		todolist[i].inData = (rand() % (MAXKEY-1))+1;
	}
	for(;i<NUMTASKS+NUMTHREADS; i++)	todolist[i].task = TERM;
	return todolist;
}
