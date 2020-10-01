//array implementation in C
#include<stdio.h>
#include<stdlib.h>
int k,i,j,pos,temp, ar[100],n;
void insert(int ar[],int n,int k,int pos);
void deletion(int ar[],int n,int k,int pos);
void sort_insert(int ar[],int n,int k);
void display(int ar[],int n);
void reverse(int ar[],int n);
    
int main(){
	int ar[100],n;
				
	do{ 
	    int choice;  
		printf("\n\n......Main Menu......\n\nEnter 0 for CREATING ARRAY\n\nEnter 1 for INSERTING AN ELEMENT\n\nEnter 2 for DELETING\n\nEnter 3 FOR INSERTING IN SORTED ARRAY\n\nEnter 4 for REVERSING\n\nENTER 5 FOR DISPLAYING ARRAY ELEMENT\n\nENTER 6 FOR EXIT\n\n");
		printf("\n\nEnter your Choice\n\n");
		scanf("%d",&choice);
		switch(choice){
			case 0:printf("\nEnter number of elements in the array\n");
				scanf("%d",&n);
				printf("\nEnter your array\n");
				for(i=0;i<n;i++)
				   scanf("%d",&ar[i]);
				break;
			case 1:
				printf("\nEnter element to be inserted\n");
				scanf("%d",&k);
				printf("\nEnter position at which element is to be inserted");
				scanf("%d",&pos);
				insert(ar,n,k,pos);
			    break;
			case 2:
				printf("\nEnter position of element to be deleted\n");
				scanf("%d",&pos);
				deletion(ar,n,k,pos);
				break;
			case 3:
				int j;
				printf("\nEnter element to be inserted in a sorted array\n");
				scanf("%d",&k);
				sort_insert(ar,n,k);
				break;
			case 4:
				reverse(ar,n);
				break;
			case 5:
				display(ar,n);
				break;
			case 6:
				printf("\nBYE!BYE! YOU EXIT\n");
				exit(0);
			default:
				printf("\nNo matches found make another choice\n");
				
		}
	}while(1);
}
void display(int ar[],int n)
{
	for(i=0;i<n;i++){
		printf("%d ",ar[i]);
	}
}
 void insert(int ar[],int n,int k,int pos){
 	if(n==100){
 		printf("\nArray overflow insertion not possible\n");
 		
	 }
 	
 	 for(i=n-1;i>=pos;i--)
        {
                ar[i+1]=ar[i];
        }
        ar[pos]=k;
        n=n+1;
        printf("\narray after inserion\n");
        display(ar,n);
        
 }
 void deletion(int ar[],int n,int k,int pos){
 	if(n<0){
 		printf("\nArray underflow\n");
 		
	 }
	else{
	
 	for(i=pos;i<n-1;i++){
 		ar[i]=ar[i+1];
	 }
	 n=n-1;
 	
 }
 printf("\nArray after deletion\n");
 display(ar,n);
}
 void sort_insert(int ar[],int n,int k){
 	if(n==100)
 	   printf("Array overflow");
    else{
	
 	      j=n-1;
 	     while(ar[j]>k&&j>=0){
 		     ar[j+1]=ar[j];
 		     j--;
	 }  
	 ar[j+1]=k;
	 n=n+1;
 printf("\nArray after insertion\n");
 display(ar,n);
}
}
 void reverse(int ar[],int n){
 	int end=n-1;
 	for(i=0;i<n/2;i++){
 		 temp=ar[i];
		 ar[i]=ar[end];
		 ar[end]=temp;
		 end--;
	 }
	 printf("\nthe array after reversing\n");
	 display(ar,n);
 }

			
			
