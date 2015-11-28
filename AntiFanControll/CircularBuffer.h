/*
 * CircularBuffer.h
 *
 * Created: 25.11.2015 21:04:45
 *  Author: Stefan
 */ 

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#define CIRCULARBUFFERSIZE 40

extern unsigned char *pRead;
extern unsigned char *pWrite;
extern unsigned char circularBuffer[CIRCULARBUFFERSIZE];
extern int bufferFreeLength;

void initCircularBuffer();
int sendUartString(unsigned char *, int);

#endif 
