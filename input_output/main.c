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
    printf("Please enter an integer: ");
    scanf("%d", &number_entered);
    printf("The number entered is: %d\n", number_entered);
    
    // Input Output using STDIN and
    // STDOUT
    char buffer[BUF_SIZE];
    printf("Entering STDIN while loop\n");
    while(fgets(buffer, BUF_SIZE, stdin)){
        puts(buffer);
        if(buffer[0] == 'q'){
            printf("You entered 'q', so I'll leave now\n");
            break;
        }
    }

    return 0;
}
