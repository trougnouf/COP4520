#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAXTHREADS 5

struct fiveBytes {
	volatile uint32_t curValue;
	volatile uint32_t isWaiting;
};


void *thread_dothethings(void* argument)
{
	//struct fiveBytes foo = *((struct fiveBytes*)argument);
	struct fiveBytes* foo = (struct fiveBytes*) argument;
	//int arg = *((int *) argument);
	//struct fiveBytes arg = *((struct fiveBytes*) argument);
	printf("%u: This thread did the thing.\n", (*(struct fiveBytes*)argument).isWaiting);
	(*(struct fiveBytes*)argument).isWaiting = 1;
	
	//printf("%d\n", *((int *) argument));
	pthread_exit(NULL);
}

/*
void *thread_dothethings(void* argument)
{

	int* arg = argument;
	printf("%u: This thread did the thing.\n", *((int*)arg));
	//printf("%d\n", *((int *) argument));
	pthread_exit(NULL);
}
*/
int main(void)
{
	struct fiveBytes structone;
	structone.curValue = 5;
	structone.isWaiting = 8;
	

	int value = 7;
	pthread_t threadone;
	pthread_create(&threadone, NULL, thread_dothethings, (void *) &structone);
	for(int i=0; i<99999; i++)	{}
	printf("%u\n", structone.isWaiting);
	pthread_join(threadone, NULL);
}
