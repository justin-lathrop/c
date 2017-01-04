/*
 * @author: jelathro
 * @date: 10/27/13
 * 
 * Queue implementation file.
 */

#include <stdio.h>
#include <stdlib.h>

#include "Queue.h"

Queue_t queue_initialize(){
	Queue_t q = { 0, 0 };
	return(q);
}

int queue_push(Queue_t * q, void * d){
	QueueItem_t * tmp;

	if(q->size == 0){
		q->head = (QueueItem_t *) malloc( sizeof(QueueItem_t) );
                if (q->head == NULL) {
                    printf("\nERROR: Insufficient memory. Terminating...");
                    exit(EXIT_FAILURE);
                }
		q->head->next = 0;
		q->head->data = d;
	}else{
		tmp = q->head;
		q->head = (QueueItem_t *) malloc( sizeof(QueueItem_t) );
                if (q->head == NULL) {
                    printf("\nERROR: Insufficient memory. Terminating...");
                    exit(EXIT_FAILURE);
                }
		q->head->data = d;
		q->head->next = tmp;
	}

	q->size++;

	return(1);
}

int queue_pop(Queue_t * q, queue_callback_func_pop cb){
	// Temporary "iterator" on the list
	QueueItem_t * nextItem = q->head;

	if(q->size == 0) return(0);

	if(q->size == 1){
		if( cb(q->head->data) ){
			free(q->head);
			q->size--;
			return(1);
		}else{
			return(0);
		}
	}

	// Move to one before end of list
	while(nextItem->next){
		if(!nextItem->next->next){
			// Remove last list item
			if( cb(nextItem->next->data) ){
				free(nextItem->next);
			}else{
				return(0);
			}

			nextItem->next = 0;

			q->size--;

			return(1);
		}else{
			nextItem = nextItem->next;
		}
	}

	return(0);
}

void * queue_front(Queue_t * q){
	QueueItem_t * tmp = q->head;

	if(q->size == 0) return( (void *)0);

	while(tmp->next){
		tmp = tmp->next;
	}

	return( tmp->data );
}

void queue_for_each(Queue_t * q, queue_callback_func cb){
	QueueItem_t * next = q->head;
	int idx = 0;

	if(q->size == 0) return;

	while(next){
		cb(idx, next->data);
		idx++;
		next = next->next;
	}
}

int queue_remove(Queue_t * q, queue_callback_func_pop cb){
	QueueItem_t * tmp;
	QueueItem_t * next = q->head;

	while(next){
		tmp = next;
		next = next->next;

		if(cb(tmp->data)){
			free(tmp);
		}else{
			return(0);
		}
	}

	return(1);
}
