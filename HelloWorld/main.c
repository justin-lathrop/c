#include <unistd.h>

/*
 * @author jelathro
 * @date 2/18/13
 * 
 * @reviewd by Abdou Developer (Abdelhakim R)
 * @https://github.com/test20091
 * @date 09/23/2018
 *
 * Print "Hello, World!" to the console
 * fputstr "Hello, World!" using native function write
 * and pointer to char
 */

void	fputchar (char c)
{
	write (1, &c, 1);
}

void	fputstr (char *str)
{
	char c;

	while (*str)
	{
		c = *str;
		fputchar(c);
		str++;
	}
}

int	main(void){
    fputstr("Hello, World!\n");
    return 0;
}
