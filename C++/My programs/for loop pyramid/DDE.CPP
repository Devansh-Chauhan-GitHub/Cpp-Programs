#include<iostream.h>
#include<conio.h>
void main()
{
clrscr();
int i,j,k,l;
for(i=1;i<=5;i++)
{
for(j=i;j<=5;j++)
{
cout<<" ";
}
for(k=1;k<=i;k++)
{
cout<<k;
}
for(l=i-1;l>=1;l--)
{
cout<<l;
}
cout<<endl;
}
getch();
}