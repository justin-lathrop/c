#include <stdio.h>
int main()
{
  for(int k=1;k<=5;k++)
  {
  for(int i=5 ;i>=k;i--)
  {
    printf(" ");
  }

    for(int j=1;j<=k;j++)
     {
       printf("*");  //if you add space triangle will be formed
     }
         printf("\n");

  }
  return 0;
}