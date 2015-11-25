/*
 * ringbuffer.c
 *
 * Created: 16.11.2015 11:36:14
 *  Author: Michael Mühlbacher
 http://c.learncodethehardway.org/book/ex44.html
 */ 

//Ringbuffer is empty when ptRead is one element before ptWrite
//Ringbuffer is full  when ptWrite is on the same element as ptRead
/*#include "ringBuffer.h"

typedef enum { false, true } bool;

RingBuffer* RingBuffer_create(int length)
{
	RingBuffer *buffer = calloc(1, sizeof(RingBuffer));
	buffer->length  = length;
	buffer->ptRead = length-1;
	buffer->ptWrite = 0;
	buffer->data = calloc(buffer->length, 1);

	return buffer;
}

void RingBuffer_destroy(RingBuffer* buffer)
{
	if(buffer) {
		free(buffer->data);
		free(buffer);
	}
}

int RingBuffer_push(RingBuffer *buffer, char *data, int length)
{
	int bytesWrote = 0;					//wieviele Bytes wurden geschrieben
	
	while((Ringbuffer_empty(buffer) == false) || bytesWrote <= length)		//so lange bis nichts mehr zu schreiben ist
	{
		buffer->data[buffer->ptWrite] = data[bytesWrote];
		Ringbuffer_incWrite();
		bytesWrote++;
	}
	return bytesWrote;
}

int RingBuffer_pop(RingBuffer* buffer, unsigned char* target, int length)	//length = number of bites to read
{
	target = (char*) malloc(sizeof(unsigned char*) * length);
	int bytesRead = 0;					//wieviele Bytes wurden gelesen
	
	while((Ringbuffer_empty(buffer) == false) || bytesRead <= length)		//so lange bis nichts mehr zu lesen ist
	{
		Ringbuffer_incRead(buffer);
		target[bytesRead] = buffer->data[buffer->ptRead];		//increment ptRead, then read
		
		bytesRead++;
	}
	return bytesRead;
}

bool Ringbuffer_isEmpty(RingBuffer* buffer)
{
	
	if(buffer->ptRead == buffer->ptWrite-1)
		return true;	//is Empty
	else if ((buffer->ptRead == buffer->length-1) && (buffer->ptWrite == 0))
		return false;
	else return 0;
}
bool Ringbuffer_isFull(RingBuffer* buffer)
{
	if(buffer->ptRead == buffer->ptWrite)
		return true;
	else return false;
}

void Ringbuffer_incRead(RingBuffer* buffer)
{
	if(buffer->ptRead == buffer->length-1) buffer->ptRead = 0;
	else buffer->ptRead++;
}
void Ringbuffer_incWrite(RingBuffer* buffer)
{
	if(buffer->ptWrite == buffer->length-1) buffer->ptWrite = 0;
	else buffer->ptWrite++;
}*/