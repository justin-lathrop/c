#include <stdio.h>

int main()
{
    float r, area;
    printf("Enter the Radius of the circle ");
    scanf("%f", &r);
    area = 3.14 * r * r;
    printf("The area of the circle is %f", area);
    return 0;
}