/*
 * CircularBuffer.c
 *
 * Created: 25.11.2015 21:04:36
 *  Author: Stefan
 */ 

#include <avr/io.h>
#include "CircularBuffer.h"

unsigned char *pRead;
unsigned char *pWrite;
unsigned char circularBuffer[CIRCULARBUFFERSIZE];
int bufferFreeLength;

int sendUartString(unsigned char *pData, int length)
{
	if(pWrite >= pRead)
	{
		bufferFreeLength = CIRCULARBUFFERSIZE - (int)pWrite + (int)pRead - 1;
	}
	else
	{
		bufferFreeLength = (int)pRead - (int)pWrite - 1;
	}
	
	if(length > bufferFreeLength) return 1;
	
	for(int i = 0; i < length; i++)
	{
		*pWrite = pData[i];
		pWrite++;
		if(pWrite >= &circularBuffer[CIRCULARBUFFERSIZE]) pWrite = circularBuffer;
	}
	
	UCSRB |= (1 << UDRIE);
	
	return 0;
} 

void initCircularBuffer()
{
	pRead = circularBuffer;
	pWrite = circularBuffer;
}



