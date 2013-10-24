#include <stdio.h>

/*
 * @author jelathro
 * @date 2/19/13
 * 
 * Provide various examples of pointers 
 * and how pointers work.
 */
int main(void){
    // Basic pointers and Basic addresses
    int *basic_pointer;
    int basic_value;

    // Give value a literal value
    basic_value = 'A';
    // Assign pointer to address
    // of literal value in value
    basic_pointer = &basic_value;

    printf("pointer is at address %p\n", &basic_pointer);
    printf("value at address of pointer is %c\n\n", *basic_pointer);

    // More advanced use of pointers
    // Array's are just fancy pointers
    char array[5] = {'a', 'A', 'b', 'z', 'M'};
    char *same_array = array;
    char **fancy_same_array = &same_array;

    printf("array[3] = %c, addr = %p\n", array[3], &array[3]);
    printf("same_array[3] = %c, addr = %p\n", same_array[3], &same_array[3]);
    printf("**fancy_same_array[3] = %c, addr = %p\n", (*fancy_same_array)[3], &(*fancy_same_array)[3]);

    // char arrays are also just a "string"
    // of characters.
    printf("array string %s, addr = %p\n", array, &array);
    printf("same array string %s, addr = %p\n", same_array, &same_array);
    printf("fancy array string %s, addr = %p\n\n", (*fancy_same_array), &(*fancy_same_array));

    // Arrays of pointers
    char *ptr_array[2];
    char *ptr1 = NULL, *ptr2 = NULL;
    char char_elements[2] = {'H', 'i'};

    ptr1 = &char_elements[1];
    ptr2 = char_elements;

    ptr_array[0] = ptr2;
    ptr_array[1] = ptr1;
    
    printf("**ptr_array = %c\n", **ptr_array);
    printf("addr at **ptr_array = %p\n", &(**ptr_array));
    printf("addr at **ptr_array[0] = %p\n\n", &(*ptr_array[0]));
    
    printf("*ptr_array[1] = %c\n", *ptr_array[1]);
    printf("addr at *ptr_array[0] = %p\n", &(**ptr_array));
    printf("addr at *ptr_array[0] = %p\n", &(**ptr_array));
    printf("addr at *ptr_array[1] = %p\n", &(*ptr_array[1]));

    return 0;
}

