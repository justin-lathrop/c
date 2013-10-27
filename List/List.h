/*
 * @author: jelathro
 * @date: 10/24/13
 * 
 * Singly linked list (as compared 
 * to a doubly linked or indexed 
 * list) that contains generic items 
 * called ListItem_t.
 */

#ifndef __LIST_H__
#define __LIST_H__

#include "ListItem.h"

typedef struct List_t{
	ListItem_t * head;
} List_t;

/*
 * Given list, Creates a new empty List_t and 
 * returns its reference.
 */
List_t list_initialize();

/*
 * Given list, Creates a new ListItem_t and adds 
 * it to the end of the List_t.  This 
 * returns an int accordingly if 
 * successful or not.
 */
int list_add_item(List_t *, ListItem_t *);

/*
 * Given list, Creates a new ListItem_t and adds 
 * it after the index given.  This returns 
 * an int accordingly if successful 
 * or not.
 */
int list_add_item_after(List_t *, int, ListItem_t *);

/*
 * Given list, Creates a new ListItem_t and adds 
 * it before the index given.  This returns 
 * an int accordingly if successful 
 * or not.
 */
int list_add_item_before(List_t *, int, ListItem_t *);

/*
 * Given list, Free's and removes head from 
 * the List_t.
 */
int list_remove_item_head(List_t *, list_item_remove_callback_func);

/*
 * Given list, Free's and removes tail from 
 * List_t.
 */
int list_remove_item_tail(List_t *, list_item_remove_callback_func);

/*
 * Given list, Free's and removes ListItem_t 
 * and updates list "hole".
 */
int list_remove_item_at(List_t *, int, list_item_remove_callback_func);

/*
 * Given list, Free's and removes list 
 * and returns int according to success.
 */
int list_remove(List_t *, list_item_remove_callback_func);

/*
 * Given list and callback function, list 
 * will loop through all LitItem_t's and 
 * call the function for it.
 */
int list_for_each(List_t *, list_item_callback_func);

/*
 * Given list and index to find and 
 * return ListItem_t data (void *).
 */
void * list_get_at(List_t *, int);

/*
 * Given list and index to find and 
 * set ListItem_t data (void *), 
 * returns int on success accordingly.
 */
int list_set_at(List_t *, void *, int);

#endif
