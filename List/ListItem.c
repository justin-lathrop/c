/*
 * 
 */

#include <stdlib.h>

#include "ListItem.h"

/*
 * Allocates new memory for a ListItem_t 
 * then returns a pointer to it.
 */
ListItem_t * list_item_initialize(){
	ListItem_t * li = (ListItem_t *) malloc( sizeof(ListItem_t) );
        
        // Making sure there was enough memory for a 'ListItem_t' type 'li'.
        if (li == NULL) {
            printf("\nERROR: Insufficient memory. Terminating...");
            exit(EXIT_FAILURE);   
        }
        
	li->next = 0;

	return(li);
}

/*
 * Removes ListItem_t data and returns 
 * if successful or not.
 */
int list_item_remove(ListItem_t * li, list_item_remove_callback_func cb){
	return( cb(li->data) );
}

/*
 * Add ListItem_t data and returns 
 * if successful or not.
 */
int list_item_add(ListItem_t * li, void * data){
	li->data = data;
	return(1);
}
