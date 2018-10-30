#include<stdio.h>
void push(int [],int,int);
void pop(int []);
int peek(int []);
void display(int []);
int top=-1;

int main()
{
    int stack[100];
    int choice,val,max;
    printf("Enter the Maximum number of Elements the stack can hold:");
    scanf("%d",&max);

    do
    {
        printf("\n1.Push");
        printf("\n2.Pop");
        printf("\n3.Peek");
        printf("\n4.Display");
        printf("\n5.EXIT");
        scanf("%d",&choice);
        switch (choice)
        {
            case 1:
                printf("Enter value into stack");
                scanf("%d",&val);
                push(stack,val,max);
                break;
            case 2:
                pop(stack);
                break;
            case 3:
                val=peek(stack);
                if(val!=-1)
                printf("The value on top of the stack is %d",val);
                break;
            case 4:
                display(stack);
        }

    }while(choice!=5);



    return 0;
}

void push(int st[],int val,int max)
{
    if(top==max-1)
    {
        printf("STACK OVERFLOW");
    }
    else
    {
        top++;
        st[top]=val;
    }
}

void pop(int st[])
{
    if(top==-1)
    {
        printf("STACK UNDERFLOW");
    }
    else
    {

        top--;
    }
}

int peek(int st[])
{
    if(top==-1)
    {
        printf("STACK IS EMPTY");
        return -1;
    }
    else
    {
        return (st[top]);
    }
}

void display(int st[])
{
    int i;
    if(top==-1)
    {
        printf("STACK IS EMPTY");

    }
    else
    {
        for(i=top;i>=0;i--)
        {
            printf("\n%d",st[i]);
        }

    }
}
