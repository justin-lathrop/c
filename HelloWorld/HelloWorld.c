#include <stdio.h>

/*
 * @author jelathro
 * @date 2/18/13
 * 
 * Print "Hello, World!" n times.
 */
int main(void){
    int n;
    printf("Choose how many %cHello, World%c: ", 34, 34);
    scanf("%d", &n);
    while(n--){
        printf("Hello, World!\n");
    }printf("\n");
    return 0;
}
