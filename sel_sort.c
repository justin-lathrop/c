//A program to perform selection sort
#include<stdio.h>
main()
{
int arr[10],i,j,n,c,k,f=0;
printf("Enter the max number\n");
scanf("%d",&n);
printf("Enter the numbers into the array\n");
for(i=0;i<n;i++)
{
scanf("%d",&arr[i]);
}
for(i=0;i<n-1;i++)
{
k=i;
for(j=i+1;j<n;j++)
{
if(arr[j]<arr[k])
{
k=j;
f=1;
}
}
if(f==1)
{
c=arr[i];
arr[i]=arr[k];
arr[k]=c;
}
}
printf("Sorted array is:\n");
for(i=0;i<n;i++)
{
printf("%d\n",arr[i]);
}
}
