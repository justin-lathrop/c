/*
Prototype of printf() function is:

int printf( const char *format , ...)
Parameter

format: This is a string that contains a text to be written to stdout.
Additional arguments: … (Three dots are called ellipses) which indicates the variable number of arguments depending upon the format string.
printf() returns the total number of characters written to stdout. Therefore it can be used as a condition check in an if condition, while condition, switch case and Macros.

*/


#include<stdio.h> 
int main() 
{ 
	if (printf("Geeks for Geeks") ) 
	{ } 
} 


/*
#include<stdio.h> 
int main(){ 
	while (!printf( "Geeks for Geeks" )) 
	{ } 
} 
*/

/*
#include<stdio.h> 
int main(){ 
	switch (printf("Geeks for Geeks" )) 
	{ } 
}
*/
/*
#include<stdio.h> 
#define PRINT printf("Geeks for Geeks") 
int main() 
{ 
	if (PRINT) 
	{ } 
} 
*/
