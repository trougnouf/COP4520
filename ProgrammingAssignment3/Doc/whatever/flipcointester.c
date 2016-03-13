#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>

#define slLEVELS 4

uint8_t flipcoins()
{
	int32_t coin = rand();
	int32_t shift = 1;
	uint8_t result = 0;
	for(uint8_t i=0; i<slLEVELS-1; i++)
	{
		if(coin&shift)	result++;
		else		break;
		shift<<=1;
	}
	return (result>=slLEVELS)?slLEVELS:result+1; // TODO remove redundant condition?
}

void main()
{
	uint8_t * results = calloc(slLEVELS, sizeof(uint8_t));
	uint8_t curResult;
	for(int i=0; i<5000; i++)
	{
		curResult = flipcoins();
		results[curResult-1] ++;
	}
	for(int i=0; i<slLEVELS; i++)
	{
		printf("%u: %u\n", i+1, results[i]);
	}
	
	uint8_t j = 8;
	for(;j>=20;j--);
	printf("%u", j);
}
