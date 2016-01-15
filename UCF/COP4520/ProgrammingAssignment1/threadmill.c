#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAXTHREADS 5

struct fiveBytes {
	uint32_t curValue;
	uint8_t isWaiting;
};

void *thread_dothethings(void *argument)
{

	int arg = *((int *) argument);
	printf("%d: This thread did the thing.\n", *((int *) argument));
	//wait(51);
	printf("%d\n", *((int *) argument));
	pthread_exit(NULL);
}

int main(void)
{

	int value = 7;
	pthread_t threadone;
	pthread_create(&threadone, NULL, thread_dothethings, (void *) &value);
	for(value = 10; value++; value < 2000000000)	{}
	value = 9;
	value = 28;
	value = 8;
	pthread_join(threadone, NULL);
}
