/*
 * @author: jelathro
 * @date: 10/24/13
 * 
 * 
 */

#ifndef __LIST_C__
#define __LIST_C__

#include <stdlib.h>

#include "List.h"

/*
 * Given list, Creates a new empty List_t and 
 * returns its reference.
 */
List_t list_initialize(){
	List_t list = { NULL };

	return list;
}

/*
 * Given list, Creates a new ListItem_t and adds 
 * it to the end of the List_t.  This 
 * returns an int accordingly if 
 * successful or not.
 */
int list_add_item(List_t * list, ListItem_t * li){
	// Temporary "iterator" on the list
	ListItem_t * nextItem = list->head;

	// No head yet
	if(!list->head){
		list->head = li;
		return(1);
	}

	// Move to the end of the list
	while(nextItem->next){
		nextItem = nextItem->next;
	}

	// Made it to the end of the list
	// add the new item to the list.
	if(!li->next){
		nextItem->next = li;
	}else{
		return(0);
	}

	return(1);
}

/*
 * Given list, Creates a new ListItem_t and adds 
 * it after the index given.  This returns 
 * an int accordingly if successful 
 * or not.
 */
int list_add_item_after(List_t * list, int idx, ListItem_t * li){
	// Temporary "iterator" on the list
	ListItem_t * nextItem = list->head;

	// Move to list index or return 1 if no such index
	while(idx > 0){
		// Insert after last item in list
		if(!nextItem->next){ // Index doesn't exist
			return(0);
		}else{
			nextItem = nextItem->next;
			idx--;
		}
	}

	// Add item after current "nextItem" pointed too.
	if(nextItem->next){ // Middle of list
		if(!li->next){
			ListItem_t * tmpItem = list_item_initialize();

			// Save next item in list
			tmpItem = nextItem->next;

			// Attach new item to current item's next pointer
			nextItem->next = li;

			// Attach "old" next item pointer 
			li->next = tmpItem;
		}else{
			return(0);
		}
	}else if(!nextItem->next){ // End of list
		if(!li->next){
			nextItem->next = li;
		}else{
			return(0);
		}
	}

	return(1);
}

/*
 * Given list, Creates a new ListItem_t and adds 
 * it before the index given.  This returns 
 * an int accordingly if successful 
 * or not.
 */
int list_add_item_before(List_t * list, int idx, ListItem_t * li){
	// Temporary "iterator" on the list
	ListItem_t * nextItem = list->head;

	// Move to right before list index or return 1 if no such index
	while(idx-1 > 0){
		// Insert after last item in list
		if(!nextItem->next){ // Index doesn't exist
			return(0);
		}else{
			nextItem = nextItem->next;
			idx--;
		}
	}

	// Add item after current "nextItem" pointed too.
	if(nextItem->next){ // Middle of list
		if(!li->next){
			ListItem_t * tmpItem = list_item_initialize();

			// Save next item in list
			tmpItem = nextItem->next;

			// Attach new item to current item's next pointer
			nextItem->next = li;

			// Attach "old" next item pointer 
			li->next = tmpItem;

			return(1);
		}else{
			return(0);
		}
	}else if(!nextItem->next){ // End of list
		if(!li->next){
			nextItem->next = li;
			return(1);
		}else{
			return(0);
		}
	}

	return(0);
}

/*
 * Given list, Free's and removes head from 
 * the List_t.
 */
int list_remove_item_head(List_t * list, list_item_remove_callback_func cb){
	if(list->head){
		// Get current head of list
		ListItem_t * tmp = list->head;

		if(list_item_remove(tmp, cb)){
			// Set next item in list to be head
			list->head = tmp->next;

			// Remove old list head
			free(tmp);

			return(1);
		}else{
			return(0);
		}
	}else{ // List is already empty
		return(1);
	}
}

/*
 * Given list, Free's and removes tail from 
 * List_t.
 */
int list_remove_item_tail(List_t * list, list_item_remove_callback_func cb){
	// Temporary "iterator" on the list
	ListItem_t * nextItem = list->head;
	
	// Move to one before end of list
	while(nextItem->next){
		if(!nextItem->next->next){
			// Remove last list item
			if(list_item_remove(nextItem->next, cb)){
				free(nextItem->next);
			}else{
				return(0);
			}

			nextItem->next = 0;

			return(1);
		}else{
			nextItem = nextItem->next;
		}
	}

	return(0);
}

/*
 * Given list, Free's and removes ListItem_t 
 * and updates list "hole".
 */
int list_remove_item_at(List_t * list, int idx, list_item_remove_callback_func cb){
	ListItem_t * nextItem, * lastItem;

	if(!list->head) return(1);
	if(idx == 0 && !list->head->next) return list_remove_item_head(list, cb);

	nextItem = list->head;
	lastItem = NULL;

	// Move to index
	while(idx-1 > 0){
		// Index does not exist
		if(!nextItem->next){
			return(0);
		}else{
			// Move to next Item
			lastItem = nextItem;
			nextItem = nextItem->next;
		}

		idx--;
	}

	// Last Item
	if(!nextItem->next){
		free(nextItem);
		return(1);
	}else{ // Middle Item
		// Connect item before and after index
		lastItem->next = nextItem->next;

		// Free unconnected item index
		free(nextItem);

		return(1);
	}
}

/*
 * Given list, Free's and removes list 
 * and returns int according to success.
 */
int list_remove(List_t * list, list_item_remove_callback_func cb){
	// Loop through entire list removing each item
	while(1){
		if( list_remove_item_head(list, cb) ){
			if(!list->head){
				break;
			}
		}else{
			return(0);
		}
	}

	return(1);
}

/*
 * Given list and callback function, list 
 * will loop through all LitItem_t's and 
 * call the function for it.
 */
int list_for_each(List_t * list, list_item_callback_func cb){
	ListItem_t * nextItem;
	int idx;

	if(!list->head) return(0);

	nextItem = list->head;
	idx = 0;

	while(nextItem){
		cb(idx, nextItem->data);

		nextItem = nextItem->next;
		idx++;
	}

	return(1);
}

/*
 * Given list and index to find and 
 * return ListItem_t data (void *).
 */
void * list_get_at(List_t * list, int idx){
	ListItem_t * nextItem;

	if(!list->head) return( (void *)1 );
	if(idx == 0) return( (void *) list->head->data );

	nextItem = list->head;
	
	// Iterate to list item at index given
	while(idx > 0){
		if(!nextItem->next){
			return( (void *) -1 );
		}else{
			nextItem = nextItem->next;
		}

		idx--;
	}

	return( nextItem->data );
}

/*
 * Given list and index to find and 
 * set ListItem_t data (void *), 
 * returns int on success accordingly.
 */
int list_set_at(List_t * list, void * data, int idx){
	ListItem_t * nextItem;

	if(!list->head) return(0);
	if(idx == 0){ list->head->data = data; return(0); }

	nextItem = list->head;

	// Iterate to list item at index given
	while(idx > 0){
		if(!nextItem->next){
			return(0);
		}else{
			nextItem = nextItem->next;
		}

		idx--;
	}

	// Set the item's new/updated data
	if(list_item_add(nextItem, data)){
		return(1);
	}

	return(0);
}

#endif
