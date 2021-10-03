#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
struct node
{
	int data;
	struct node *next;
};
struct node *front=NULL;
struct node *rear=NULL;

void insert(int);
void del_element();
void display();

int main()
{
	int n,option;
	do
	{
		printf("\n\n*************QUEUE MENU************");
		printf("\n\n1.ADD");
		printf("\n2.REMOVE");
		printf("\n3.DISPLAY");
		printf("\n4.EXIT");
		printf("\n\n enter your option:");
		scanf("%d",&option);
		switch(option)
		  {
			case 1:
				printf("\n enter the number");
				scanf("%d",&n);
				insert(n);                                      //insert function call
				break;
			case 2:
			del_element();                                      //del_element function call
			break;
			case 3:
			display();                                          //display function call
			break;
			default:
				{
				printf("\nSorry u have entered a wrong choice . beter Luck next time\n");
				exit(0);
				}
				
		  }
		}
		while(option !=4);
	}
void insert(int item)
{
	struct node *ptr=malloc(sizeof(struct node));
	ptr->data=item;
	ptr->next=NULL;
	if(rear==NULL)
	{
		front=ptr;
		rear=ptr;
	}
	else
	{
		rear->next=ptr;
		rear=rear->next;
	}
}
void del_element()
{
	if(front==NULL)
	{
		printf("\n\nQUEUE IS EMPTY ");
	}
	else
	{
		struct node *temp;
		temp=front;
		front=front->next;
		printf("\nDELETE=%d",temp->data);
		free(temp);
	}
}
void display()
{
	struct node *temp;
	temp=front;
	printf("\n");
	while(temp != NULL)
	{
		printf("%d\n",temp->data);
		temp=temp->next;
	}
}


