//WAP to check given string or number is palindrome or not
#include<stdio.h>
#include<stdlib.h>
char str[50];
int n,flag;
void select1();
void obj();
void rev(int);
void coun_obj();
void peli(int,char[]);
void control();
int main()
{
	printf("\nWelcome to my program\n");
	select1();
}
void select1()
{
	int ch;
	printf("\nEnter 1 to perform operation on Digit\nEnter 2 to perform operation on String\n");

	scanf("%d",&ch);
	switch(ch)
	{
		case 1:
		{
			flag=0;
			obj();
			break;
		}
		case 2:
		{
			flag=1;
			obj();
			break;		
		}
		default:
		{
			printf("\nKeyword is not valid\n");
		}
	}
}
void obj()
{
	if(flag==0)
	{
		printf("\nEnter any number\n");
		scanf("%d",&n);
		coun_obj();
	}
	else
	{
		printf("\nEnter string\n");
		scanf("%s",str);
		coun_obj();
	}
}
void coun_obj()
{
	int count,i;
	if(flag==0)
	{
		int temp;
		temp=n;
		for(count=0;n>0;count++)
		{
			n=n/10;
		}
		n=temp;
		count=count-1;
		rev(count);
	}
	else
	{
		for(i=0;str[i]!='\0';i++)
		{
			count++;
		}
		rev(count);
	}
}
void rev(int count)
{
	char rev[50];
	int rev1=0;
	if(flag==0)
	{
		int temp,c,ori;
		ori=n;
		while(n>0)
		{
			temp=n%10;
			n=n/10;
			c=count;
			while(count>0)
			{
				temp=temp*10;
				count--;
			}
			count=c;
			rev1=rev1+temp;
			count--;
		}	
		n=ori;
	}
	else
	{
		char temp;
		int i,j;
		for(i=count-1,j=0;str[i]!='\0';i--,j++)
		{
			rev[j]=str[i];
		}
	}
	peli(rev1,rev);
}
void peli(int rev1,char rev[])
{
	int i;
	if(flag==0)
	{
		if(n==rev1)
		{
			printf("Given number is pelindrom\n");
		}
		else
		{
			printf("Given number is not pelindrom\n");
		}
	}
	else
	{
		int f=0,j;
		for(i=0;str[i]!='\0';i++)
		{
			if(str[i]!=rev[i])
			{
				f=1;
				printf("\nString is not pelindrom\n");
				break;
			}
		}
		if(f==0)
		{
			printf("\nString is pelindrom\n");
		}
	}	
	control();
}
void control()
{
	int re;
	printf("\nEnter 1 to perform more operation\nEnter 2 for exit\n");
	scanf("%d",&re);
	switch(re)
	{
		case 1:
		{
			select1();
			break;
		}
		case 2:
		{
			exit(0);
			break;
		}
		default:
		{
			printf("\nEnter valid keyword\n");
			break;
		}
	}
}
