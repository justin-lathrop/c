/*
 * @author: jelathro
 * @date: 10/24/13
 * 
 * Example program using the 
 * generic list data structure 
 * implemented in List.h.
 */

#include <stdio.h>
#include <stdlib.h>

#include "List.h"

void print_list_item(int idx, void * data){
	printf("%d: %d, ", idx, (int) data);
}

int delete_list_item(void * data){
	return(1);
}

int main(int argc, char ** argv){
	int data = 0;
	List_t list_int;
	ListItem_t * li;

	// Create an empty list
	list_int = list_initialize();

	// Add items to list of type int casted 
	// to void * for the list to operate with it.
	for(data=0; data < 10; data++){
		// Add item to list ( adds to the end )
		li = list_item_initialize();
		list_item_add( li, (void *) data );
		list_add_item(& list_int, li);
	}

	// Print contents of the list
	list_for_each(& list_int, print_list_item);
	printf("\n=============================\n");

	// Add item after first one
	li = list_item_initialize();
	list_item_add(li, (void *) -1 );
	list_add_item_after(& list_int, 0, li);

	// Add item before second one
	li = list_item_initialize();
	list_item_add(li, (void *) -2 );
	list_add_item_before(& list_int, 2, li);
	
	// Print contents of the list
	list_for_each(& list_int, print_list_item);
	printf("\n=============================\n");

	list_remove_item_head(& list_int, delete_list_item);
	list_remove_item_tail(& list_int, delete_list_item);
	list_remove_item_at(& list_int, 5, delete_list_item);

	list_set_at(& list_int, (void *) 100, 7);
	
	// Print contents of the list
	list_for_each(& list_int, print_list_item);
	printf("\n=============================\n");

	// Remove the list
	list_remove(& list_int, delete_list_item);

	return(0);
}
