#include<stdio.h>
#include<stdlib.h>

#define CAPACITY 5

//Stack follows LAST IN FIRST OUT principle

int stack[CAPACITY],top = -1;

//stack functions
void push(int);
int isFull();
int pop();
int isEmpty();
void peek();
void traverse();

int main()
{
    int choice,element;

    //while loop for a menu driven program
    //to do stack operations
    while(1)
        {
    printf("\n1.Push\n");
    printf("2.Pop\n");
    printf("3.Peek\n");
    printf("4.Traverse\n");
    printf("5.Exit\n");
    printf("Enter Your Choice: ");
    scanf("%d",&choice);

    switch(choice)
    {
       case 1: printf("\nEnter the Element: ");
               scanf("%d",&element);
               push(element);
               break;

       case 2: element = pop();
               if(element == 0)
               {
                   printf("Stack is empty !\n");
               }
               else
               printf("%d has been removed.\n",element);
               break;

       case 3: peek();
               break;

       case 4: traverse();
               break;

       case 5: exit(0);

       default: printf("Invalid INPUT\n");

    }
        }
    return 0;
}

//function to push an element in stack
void push(int element)
{
    if(isFull())
    {
        printf("Stack is Full\n");
    }
    else
    {
        top++;
        stack[top] = element;
        printf("%d has been inserted.\n",element);
    }
}

//function to check whether stack is full or not
int isFull()
{
    if(top == CAPACITY - 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//function to pop an element from stack
int pop()
{
    if(isEmpty())
    {
        return 0;
    }
    else
    {
        return stack[top--];
    }
}

//function to check whether stack is empty or not
int isEmpty()
{
    if(top == -1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//function to peek the top element of stack
void peek()
{
    if(isEmpty())
    {
        printf("Stack is Empty !\n");
    }
    else
    {
        printf("peek element is %d \n",stack[top]);
    }

}

//function to traverse through stack and display the Elements
void traverse()
{
    int i;
    if(isEmpty())
    {
        printf("Stack is Empty !\n");
    }
    else
    {
        for(i=0; i<=top; i++)
        {
            printf("%d\n",stack[i]);
        }
    }
}
