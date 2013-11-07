/*
 * @author: jelathro
 * @date: 11/6/13
 * 
 * A LIFO (Last In First Out) 
 * data-structure of generic 
 * items.
 */

#include <stdio.h>
#include <stdlib.h>

#include "CircularBuffer.h"

void destroy_function(void * data){
	data = 0;
}

int main(int argc, char ** arcv){
	CircularBuffer * cb = circularbuffer_initialize(10, (void *)'0');
	int i;

	// Load buffer, plus one extra
	for(i=0; i<20; i++){
		circularbuffer_add( cb, (void *)('A' + i) );
	}

	// Read the buffer
	for(i=0; i<10; i++){
		printf( "%d: %c\n", i, circularbuffer_read(cb) );
	}

	// Destroy buffer
	circularbuffer_destroy(cb, destroy_function);

	// Destroy local handle
	free(cb);

	return(0);
}
