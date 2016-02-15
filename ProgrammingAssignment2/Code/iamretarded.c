/*
Sandbox
*/

//#include "testcase1.h"

//#include "test2.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define slLEVELS 16
uint8_t coinflipper();
uint8_t fancycoinflipper();

int main()
{
	int coin = rand();
	printf("%u\n", coinflipper());
	printf("%u\n", coinflipper());
}

uint8_t coinflipper()
{
	int coin = rand();
	int shift = 1;
	uint8_t result = 0;
	for(uint8_t i=0; i<32; i++)
	{
		if(coin&shift)	result++;
		else		return result;
		shift<<=1;
	}
	return result;
}

uint8_t fancycoinflipper()
{
	return (uint8_t)(1230 % ((slLEVELS * 2)+1));
}
