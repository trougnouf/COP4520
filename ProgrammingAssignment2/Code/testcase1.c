#include "testcase1.h"
#include "handler.h"



Task * loadTestCase()
{
	Task * todolist = malloc(NUMTASKS * sizeof(Task));
	
	State randTask;
	char randNum;
	for(int i=0; i < NUMTASKS; i++)
	{
		// set tasks here
		randNum = rand() % 101;
		if(randNum <= PERCENTINSERT)			randTask=INSERT;
		else if(randNum <= PERCENTINSERT+PERCENTFIND)	randTask=FIND;
		else						randTask=REMOVE;
		todolist[i].task = randTask;
		todolist[i].inData = rand() % 1073741824;
		
	}
	return todolist;
}
