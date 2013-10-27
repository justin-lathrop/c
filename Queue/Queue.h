/*
 * @author: jelathro
 * @date: 10/27/13
 * 
 * Queue implementation header 
 * for what is available.
 */

#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef int (* queue_callback_func)(int, void *);
typedef int (* queue_callback_func_pop)(void *);

typedef struct QueueItem_t{
	struct QueueItem_t * next;
	void * data;
} QueueItem_t;

typedef struct Queue_t{
	struct QueueItem_t * head;
	int size;
} Queue_t;

/*
 * Creates and returns a new 
 * Queue_t instance.
 */
Queue_t queue_initialize();

/*
 * Adds a QueueItem_t of any value 
 * to the back of the queue.
 */
int queue_push(Queue_t *, void *);

/*
 * Removes and returns the first item added 
 * to the queue (FIFO).
 */
int queue_pop(Queue_t *, queue_callback_func_pop);

/*
 * Returns the data in the next item 
 * in the queue.
 */
void * queue_front(Queue_t *);

/*
 * Not really a queue function, nice for 
 * debugging though. :-)
 */
void queue_for_each(Queue_t *, queue_callback_func);

/*
 * Removes all data in the queue and returns 
 * if it was successful.
 */
int queue_remove(Queue_t *, queue_callback_func_pop);

#endif
