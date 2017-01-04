/*
 * @author: jelathro
 * @date: 11/6/13
 */

#include <stdlib.h>

#include "CircularBuffer.h"

CircularBuffer * circularbuffer_initialize(size_t size, void * val){
	size_t i;
	CircularBuffer * cb = (CircularBuffer *)malloc( sizeof(CircularBuffer) );
        
        if (cb == NULL) {
            
            // any other implementation may be added here.
            
            printf("\nERROR: Insufficient memory. Terminating...");
            exit(EXIT_FAILURE);
        }

	cb->buffer = (Item *)calloc(size, sizeof(Item));
        
        if (cb->buffer == NULL) {
            
            // any other implementation may be added here.
            
            printf("\nERROR: Insufficient memory. Terminating...");
            exit(EXIT_FAILURE);
        }

	for(i=0; i<size; i++){
		cb->buffer[i].data = val;
	}

	cb->size = size;
	cb->start = 0;
	cb->end = 0;

	return(cb);
}

int circularbuffer_add(CircularBuffer * cb, void * val){
	cb->buffer[ cb->end ].data = val;
	cb->end = (cb->end + 1) % cb->size;

	if( cb->end == cb->start){
		cb->start = (cb->start + 1) % cb->size;
	}

	return(1);
}

void * circularbuffer_read(CircularBuffer * cb){
	size_t start = cb->start;
	cb->start = (cb->start + 1) % cb->size;

	return( cb->buffer[ start ].data );
}

int circularbuffer_destroy(CircularBuffer * cb, circularbuffer_destroybuffer df){
	size_t i;

	for(i=0; i<cb->size; i++){
		df( cb->buffer[i].data );
	}

	free(cb->buffer);
	return(1);
}
