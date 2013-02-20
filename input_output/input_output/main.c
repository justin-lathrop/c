#include <stdio.h>

#define BUF_SIZE 1024

/*
 * @author jelathro
 * @data 3/19/13
 * 
 * Show some of the various input output
 * (I/O) methods in C.
 */
int main(void){
    // Input Output using printf
    // and scanf
    int number_entered = 0;
    printf("number_entered = %d\n", number_entered);
    scanf("%d", &number_entered);
    printf("nmber_entered = %d\n", number_entered);
    
    // Input Output using STDIN and
    // STDOUT
    char buffer[BUF_SIZE];
    printf("Entering STDIN while loop\n");
    while(fgets(buffer, BUF_SIZE, stdin)){
        puts(buffer);
    }
    printf("You pressed control+d, so i'll leave now\n");

    return 0;
}
