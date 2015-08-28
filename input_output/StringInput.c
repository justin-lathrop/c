#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1024

/*
 * @author deshant
 * @data 8/22/15
 *
 * Taking input without overflow of buffer
 * (I/O) in C.
 */

char * inputLine(size_t, char);

int main(void){
    printf("Enter a string: ");

    char* line = inputLine(BUF_SIZE,'\n');
    printf("You Entered: %s",line);

    free(line);
    return(0);
}

char * inputLine(size_t size, char delimeter){
    char * buffer = NULL;
    size_t read;

    read = getdelim(&buffer, &size, delimeter, stdin);
    if(-1 != read){
        return(buffer);
    }else{
        free(buffer);
        return(NULL);
    }
}

/*
char* inputLine(size_t size, char delimiter){
    char* str;
    int ch;
    size_t len = 0;
    str = malloc(NULL, sizeof(char) * size);
    if(!str)
    	return str;

    while(EOF!=(ch=fgetc(stdin)) && ch != delimiter){
    	if(ch == '\t')
    		continue;
        str[len++]=ch;
        if(len==size){
            str = realloc(str, sizeof(char) * (size+=32));
            if(!str)return str;
        }
    }

    if(ch == EOF && len == 0)
    {
        // Ability to check EOF easily
    	str[0] = EOF;
    	return str;
    }

    // Adding null at the end
    str[len++]='\0';
    return realloc(str, sizeof(char) * len);
}
*/

