/*
 * ringbuffer.h
 *
 * Created: 16.11.2015 11:34:03
 *  Author: Michael Mühlbacher
 */ 

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

typedef struct {
	unsigned char *data;
	int length;
	int ptRead;
	int ptWrite;
} RingBuffer;

RingBuffer* RingBuffer_create(int length);
void RingBuffer_destroy(RingBuffer* buffer);

int RingBuffer_push(RingBuffer* buffer, char* data, int length);
int RingBuffer_pop(RingBuffer* buffer, char *target, int amount);


int Ringbuffer_isEmpty(RingBuffer* buffer);
int Ringbuffer_isFull(RingBuffer* buffer);

void Ringbuffer_incRead(RingBuffer* buffer);
void Ringbuffer_incWrite(RingBuffer* buffer);

#endif /* RINGBUFFER_H_ */