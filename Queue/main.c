/*
 * @author: jelathro
 * @date: 10/27/13
 * 
 * Example code use for this 
 * pure c queue library implementation.
 */

#include <stdio.h>

#include "Queue.h"

// Queue for each function callback
int print_queue(int idx, void * data){
	printf("%d: %c,", idx, (char *) data);

	return(1);
}

// Queue destroy queue item function callback
int destroy_queue_item(void * data){
	data = 0;

	return(1);
}

int main(int argc, char ** argv){
	int i = 0;

	// Create Queue_t instance
	Queue_t queue = queue_initialize();

	// Add some items in the queue
	for(i=0; i < 5; i++){
		queue_push( & queue, (void *)('a' + i) );
	}

	// Print out the queue (only debugging)
	queue_for_each(& queue, print_queue);
	printf("\n==============================\n");

	// Start popping some items
	for(i=0; i < 5; i++){
		queue_pop(& queue, destroy_queue_item);
	}

	// Print the queue again
	queue_for_each(& queue, print_queue);
	printf("\n==============================\n");

	// Push item to empty list and view the "front" 
	// which is the item pushed in this case.
	queue_push(& queue, (void *) 'z');
	printf("Front: %c\n", (char *) queue_front(& queue));

	// Delete queue
	queue_remove(& queue, destroy_queue_item);

	return(0);
}
