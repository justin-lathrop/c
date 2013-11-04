/*
 * @author: jelathro
 * @date: 11/2/13
 * 
 * Implementation file for HashTable.
 * 
 * After ten collisions (ie. ten items in 
 * a particular hash items list) the hash 
 * table will expand to the maxCollision 
 * plus the growthFactor.
 */

#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

typedef size_t (* hash_function)(void *);
typedef size_t (* compare_equal)(void *, void *);

typedef struct Item{
	void * key;
	void * value;
	struct Item * next;
} Item;

typedef struct HashTable{
	size_t size;
	size_t maxCollisions;
	size_t growthFactor;
	Item ** table;
	hash_function hf;
	compare_equal eq;
} HashTable;

/*
 * Creates instance of HashTable with size and 
 * function given to create a hash of the key.
 * 
 * The hash function is given so that any variable 
 * or structure etc can be used as the key in the 
 * key value pairing.
 */
HashTable * hashtable_initialize(size_t, size_t, size_t, hash_function, compare_equal);

/*
 * Finds the value in the table in O(n) speed given 
 * key, and returns value as void * or void * 0 on 
 * error.
 */
void * hashtable_get(HashTable *, void *);

/*
 * Free's all memory in the hash table.  Returns 
 * success status 1 true, 0 false.
 */
int hashtable_destroy(HashTable *);

/*
 * Resizes the hash table to a different 
 * size.
 */
int hashtable_resize(HashTable *, size_t);

/*
 * Adds a new key value pair to the table.  If 
 * key already exists it will overwrite it.
 */
int hashtable_add(HashTable *, void *, void *);

/*
 * Free's and deletes memory of a item and clears 
 * the spot in the table.
 */
int hashtable_remove(HashTable *, void *);

#endif
