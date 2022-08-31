
#include <stdio.h>

int main()
{
  int n;
  printf("Enter the size of array: "); 
  scanf("%d", &n);                      //accept the size of your array
  
  int arr[n];                          //declaring array of size n
  
  for(int i=0;i<n;i++){                 
    scanf("%d", &arr[i]);              //accepting the elements of array from user
  }
  
  for(int i=0;i<n;i++){
      printf("%d ", arr[i]);           //print all the elements of array
  }

    return 0;
}
