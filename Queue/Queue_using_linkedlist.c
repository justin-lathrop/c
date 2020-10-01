#include<stdio.h>
#include<stdlib.h>
 
struct node
{
      int data;
      struct node *link;
}*front = NULL, *rear = NULL;
 
int isEmpty()
{
      if(front == NULL)
      {
            return 1;
      }
      else
      {
            return 0;
      }
}
 
void insert(int item)
{
      struct node *temp;
      temp = (struct node *)malloc(sizeof(struct node));
      if(temp == NULL)
      {
            printf("Queue is not Allocated\n");
            return;
      }
      temp->data = item;
      temp->link = NULL;
      if(front == NULL)
      {
            front = temp;
      }
      else
      {
            rear->link = temp;
      }
      rear = temp;
}
 
int delete_node()
{
      struct node *temp;
      int item;
      if(isEmpty())
      {
            printf("Queue is Empty\n");
            exit(1);
      }
      temp = front;
      item = temp->data;
      front = front->link;
      free(temp);
      return item;
}
 
int peek()
{
      if(isEmpty())
      {
            printf("Queue is Empty\n");
            exit(1);
      }
      return front->data;
}
 
void display()
{
      struct node *ptr;
      if(isEmpty())
      {
            printf("Queue is Empty\n");
            return;
      }
      printf("Queue Elements (or Nodes):\n");
      for(ptr = front; ptr != NULL; ptr = ptr->link)
      {
            printf("%d ", ptr->data);
      }
      printf("\n\n");
}
 
int main()
{
      int option, item;
      while(1)
      {       
            printf("1. Insert an Element (Node) in the Queue\n");
            printf("2. Delete an Element from the Queue\n");
            printf("3. Display Element at the Front position\n");
            printf("4. Display All Elements of the queue\n");
            printf("5. Exit\n");
            printf("Enter your option:\t");
            scanf("%d", &option);
            switch(option)
            {
                  case 1:
                          printf("Enter the Element to Add in Queue:\t");
                          scanf("%d", &item);
                          insert(item);
                          break;
                  case 2:
                          printf("The Deleted Element fromt the Queue:\t%d\n", delete_node());
                          break;
                  case 3:
                          printf("Element at the Front:\t%d\n", peek());
                          break;
                  case 4:
                          display();
                          break;
                  case 5:
                          exit(1);
                 default:
                          printf("Wrong option\n");
            }
      }
      return 0;
}
