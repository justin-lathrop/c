#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main()
{
    int ranNum;
    time_t t;
    srand((unsigned)time(&t));
    ranNum = rand()%21;
    int i,guess;
    printf("welcome\n");
    printf("I have chosen something from 1-20, guess what that might be, you are to get only 5 chances!!\n");
    for(i=1;i<=5;i++)
    {
        printf("make a choice\n");
        scanf("%d",&guess);
        if(guess>=1&&guess<=20)
        {
            if(guess==ranNum)
            {
                printf("you are a genius!!\n");
                break;
            }
            if(guess>ranNum)
            {
                printf("my number's smaller!\n");
            }
            if(guess<ranNum)
            {
                printf("my number's bigger!\n");
            }
        }
        printf("beware!! you have %d chances left..\n",5-i);
    }

    printf("the answer was..... %d",ranNum);


    return 0;
}
