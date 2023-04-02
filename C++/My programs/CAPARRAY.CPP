#include<iostream.h>
#include<conio.h>
#include<ctype.h>
void main()
{
clrscr();
char str[80];
int i;
cout<<"Enter the string :=";
cin.getline(str,80);
str[0]=toupper(str[0]);
for(i=0;str[i]!='\0';i++)
{
if(str[i]==' ')
{
str[i+1]=toupper(str[i+1]);
}
}
cout<<"updated string is : "<<str;
getch();
}