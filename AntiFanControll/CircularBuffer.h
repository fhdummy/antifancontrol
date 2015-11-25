/*
 * CircularBuffer.h
 *
 * Created: 25.11.2015 21:04:45
 *  Author: Stefan
 */ 

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

struct buffer {
	unsigned char volatile head_pos;
	unsigned char volatile empty_pos;
	unsigned char volatile length;
	char volatile buffer[];
};

static struct buffer txBuffer;

static void initBuffer();

#endif 
