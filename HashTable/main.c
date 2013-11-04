/*
 * @author: jelathro
 * @date: 11/2/13
 * 
 * Test/example use/use case 
 * for HashTable implementation.
 */

#include <stdio.h>

#include "HashTable.h"

size_t hash(void * key){
	size_t hash = 0;
	const char * k = (const char *)key;
	
	while (*k) {
		hash = hash*37 + *k++;
	}
	return hash;
}

size_t mycompare_equal(void * key1, void * key2){
	if((const char *)key1 == (const char *)key2)
		return(1);
	else
		return(0);
}

int main(int argc, char ** argv){
	HashTable * ht = hashtable_initialize(1, 1, 2, hash, mycompare_equal);

	hashtable_add(ht, (void *)"1", (void *)"ONE");
	hashtable_add(ht, (void *)"2", (void *)"TWO");
	hashtable_add(ht, (void *)"3", (void *)"THREE");
	hashtable_add(ht, (void *)"4", (void *)"FOUR");
	
	printf("ht[\"1\"]: %s\n", (const char *)hashtable_get(ht, (void *)"1"));
	printf("ht[\"2\"]: %s\n", (const char *)hashtable_get(ht, (void *)"2"));
	printf("ht[\"3\"]: %s\n", (const char *)hashtable_get(ht, (void *)"3"));
	printf("ht[\"4\"]: %s\n", (const char *)hashtable_get(ht, (void *)"4"));

	// Add more to inact resize

	hashtable_add(ht, (void *)"5", (void *)"FIVE");
	hashtable_add(ht, (void *)"6", (void *)"SIX");
	hashtable_add(ht, (void *)"7", (void *)"SEVEN");
	hashtable_add(ht, (void *)"8", (void *)"EIGHT");
	
	printf("ht[\"5\"]: %s\n", (const char *)hashtable_get(ht, (void *)"5"));
	printf("ht[\"6\"]: %s\n", (const char *)hashtable_get(ht, (void *)"6"));
	printf("ht[\"7\"]: %s\n", (const char *)hashtable_get(ht, (void *)"7"));
	printf("ht[\"8\"]: %s\n", (const char *)hashtable_get(ht, (void *)"8"));

	hashtable_destroy(ht);

	return(0);
}
