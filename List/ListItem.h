/*
 * @author: jelathro
 * @date: 10/24/13
 * 
 * Generic container called ListItem_t 
 * for template.  Has a pointer to the 
 * next item in the list and if its at 
 * the end of the list then it will be 
 * NULL.
 * 
 * Implements only "create" and "remove" 
 * functionality.  This is because the 
 * ListItem_t is too generic to implement 
 * anymore, the user will need to do the 
 * rest.
 */

#ifndef __LISTITEM_H__
#define __LISTITEM_H__

typedef void (* list_item_callback_func)(int, void *);
typedef int (* list_item_remove_callback_func)(void *);

typedef struct ListItem_t{
	struct ListItem_t * next;
	void * data;
} ListItem_t;

/*
 * Allocates new memory for a ListItem_t 
 * then returns a pointer to it.
 */
ListItem_t * list_item_initialize();

/*
 * Removes ListItem_t data and returns 
 * if successful or not.
 */
int list_item_remove(ListItem_t *, list_item_remove_callback_func);

/*
 * Add ListItem_t data and returns 
 * if successful or not.
 */
int list_item_add(ListItem_t *, void *);

#endif
