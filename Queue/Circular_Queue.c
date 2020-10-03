/*
 * @author: amberkakkar01
 * @date: 10/03/20
 * 
 * Circular Queue implementation file.
 */

#include<stdio.h>   
#include<stdlib.h>  
#define maxsize 5  
void insert();  
void delete();  
void display();  
int front = -1, rear = -1;  
int queue[maxsize];  
void main ()  
{  
    int choice;   
    while(choice != 4)   
    {     
        printf("\n Main Menu: \n");  
        printf("\n\n");
        printf("\n1.Insert an element\n2.Delete an element\n3.Display the queue\n4.Exit\n");  
        printf("\nEnter your choice ?");  
        scanf("%d",&choice);  
        switch(choice)  
        {  
            case 1:  
            insert();  
            break;  
            case 2:  
            delete();  
            break;  
            case 3:  
            display();  
            break;  
            case 4:  
            exit(0);  
            break;  
            default:   
            printf("\nEnter valid choice??\n");  
        }  
    }  
}  
void insert()  
{  
    int item;  
    printf("\nEnter the element\n");  
    scanf("%d",&item);    
    if((rear+1)%maxsize == front)  
    {  
        printf("\nOVERFLOW");  
        return;  
    }  
    else if(front == -1 && rear == -1)  
    {  
        front = 0;  
        rear = 0;  
    }  
    else if(rear == maxsize -1 && front != 0)   
    {  
        rear = 0;  
    }  
    else   
    {  
        rear = (rear+1)%maxsize;  
    }  
    queue[rear] = item;  
    printf("\nValue inserted ");  
}  
void delete()  
{  
    int item;   
    if(front == -1 & rear == -1)  
    {  
        printf("\nUNDERFLOW\n");  
        return;  
    }  
    else if(front == rear)  
    {  
        front = -1;  
        rear = -1;  
    }  
    else if(front == maxsize -1)  
        {  
            front = 0;  
        }  
    else   
        front = front + 1;  
}  
      
void display()  
{  
   int i;        
   if(front == -1)  
      printf("\nCircular Queue is Empty!!!\n");  
   else  
   {  
      i = front;  
      printf("\nCircular Queue Elements are : \n");  
      if(front <= rear){  
     while(i <= rear)  
        printf("%d %d %d\n",queue[i++],front,rear);  
      }  
      else{  
     while(i <= maxsize - 1)  
        printf("%d %d %d\n", queue[i++],front,rear);  
     i = 0;  
     while(i <= rear)  
        printf("%d %d %d\n",queue[i++],front,rear);  
      }  
   }  
} 