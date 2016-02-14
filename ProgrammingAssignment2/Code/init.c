#include "init.h"
//#include "skiplist.h"

void* thread_structureHandler(void* args);

typedef enum {INSERT, DELETE, FIND, IDLE} tasks;

typedef struct
{
	tasks curTask;
	int inputData;
	char resultFlag;	// 'w' = waiting
	slNode * resultNode;
} pthreadData;

void initializeTheThings()
{
	slInit();

}
