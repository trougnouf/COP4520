#include "testcase1.h"
#include "handler.h"


#define PERCENTINSERT 50
#define PERCENTFIND 25
#define PERCENTREMOVE 25

Task * loadTestCase()
{
	Task * todolist = malloc(NUMTASKS * sizeof(Task));
	
	State randTask;
	for(int i=0; i < NUMTASKS; i++)
	{
		// set tasks here
		randTask = rand() % 101;
		if(randTask <= PERCENTINSERT)			randTask=INSERT;
		else if(randTask <= PERCENTINSERT+PERCENTFIND)	randTask=FIND;
		else						randTask=REMOVE;
		todolist[i].task = randTask;
		todolist[i].inData = rand() % 1073741824;
		
	}
	return todolist;
}
