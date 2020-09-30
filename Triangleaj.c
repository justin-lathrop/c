/*
Write a C program to check whether a triangle can be formed by the given input for the angles.
The triangle is valid or not valid.
*/
#include<stdio.h>
main()
{
    int x,y,z;
    printf("Enter the value of first angle \t= ");
    scanf("%d",&x);
    printf("Enter the value of Second angle\t= ");
    scanf("%d",&y);
    printf("Enter the value of third angle \t= ");
    scanf("%d",&z);
    if (x+y+z==180)
    {
        printf("This is valid triangle\n");
    }
    else
    {
        printf("This is not valid triangle\n");
    }
    if (x==y && y==z)
    {
        printf("This is Equilateral triangle");
    }
    else if (x==90 || y==90 || z==90)
    {
        printf("This is Right triangle");
    }
    else if (x==y || y==z || z==x)
    {
        printf("This is Isosceles triangle");
    }
    else
    {
        printf("This is Scalene triangle");
    }
    return 0;

}

