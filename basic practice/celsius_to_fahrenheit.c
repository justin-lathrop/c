#include <stdio.h>

int main(){
    float celsius,fahrenheit;
    printf("Enter the degree in celsius ");
    scanf("%f",&celsius);
    fahrenheit=celsius*1.8+32;
    printf("The degree of celsius to fahrenheit is %f",fahrenheit);
    return 0;
}