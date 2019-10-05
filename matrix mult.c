#include<stdio.h>
void main(){
int a[50][50],b[50][50],c[50][50],i,j,k,m,n;
for (i=0;i<2;i++){
    for(j=0;j<2;j++){
        scanf("%d",&a[i][j]);
    }
}
for (i=0;i<2;i++){
    for(j=0;j<2;j++){
        scanf("%d",&b[i][j]);
    }}

for(i=0;i<2;i++){
    for(j=0;j<2;j++){
        for(k=0;k<2;k++){
                c[i][j]=a[i][k]*b[k][j];
        }
        }
}

for(i=0;i<2;i++){
    for(j=0;j<2;j++){
        printf("%d\t",c[i][j]);
    }
    printf("\n");
}
}
