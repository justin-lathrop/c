#include <stdio.h>

void mergeSort(int[],int,int); 
void merge(int[],int,int,int);

void display(int arr[], int size){
    int i;
    for(i = 0; i < size; i++){
        printf("%d ",arr[i]);
    }
    printf("\n");
}

void main() 
{
    int a[10]= {11, 9, 6, 19, 33, 64, 15, 75, 67, 88}; 
    int i; 

    int size = sizeof(a)/sizeof(a[0]);
    display(a, size);

    mergeSort(a,0,size-1); 
    display(a, size);
}

void mergeSort(int a[], int strt, int end)
{
    int mid;
    if(strt<end)
    {
        mid = (strt+end)/2;

        mergeSort(a,strt,mid);
        mergeSort(a,mid+1,end);
        merge(a,strt,mid,end);
    }
}

void merge(int a[], int strt, int mid, int end)
{
    int i=strt,j=mid+1,p,index = strt;
    int temp[10];

    while(i<=mid && j<=end)
    {
        if(a[i]<a[j])
        {
            temp[index] = a[i];
            i = i+1;
        }
        else
        {
            temp[index] = a[j];
            j = j+1;
        }

        index++;
    }
    if(i>mid)
    {
        while(j<=end)
        {
            temp[index] = a[j];
            index++;
            j++;
        }
    }
    else
    {
        while(i<=mid)
        {
            temp[index] = a[i];
            index++;
            i++;
        }
    }
    p = strt;
    while(p<index)
    {
        a[p]=temp[p];
        p++;
    }
}
