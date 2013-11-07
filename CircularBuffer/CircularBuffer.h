/*
 * @author: jelathro
 * @date: 11/6/13
 * 
 * LIFO, in this case, easily 
 * could be FIFO.  But this is 
 * traditionally a LIFO.
 */

#ifndef __CIRCULARBUFFER_H__
#define __CIRCULARBUFFER_H__

typedef void (* circularbuffer_destroybuffer)(void *);

typedef struct Item{
	void * data;
} Item;

typedef struct CircularBuffer{
	size_t size;
	size_t start;
	size_t end;
	Item * buffer;
} CircularBuffer;

/*
 * Returns initialized CircularBuffer instance of 
 * size size_t and all items initialized to second 
 * parameter.
 */
CircularBuffer * circularbuffer_initialize(size_t, void *);

/*
 * Overwrites oldest item in the buffer.
 */
int circularbuffer_add(CircularBuffer *, void *);

/*
 * Returns oldeset item in the buffer.
 */
void * circularbuffer_read(CircularBuffer *);

/*
 * Destroys buffer entirely.
 */
int circularbuffer_destroy(CircularBuffer *, circularbuffer_destroybuffer);

#endif
