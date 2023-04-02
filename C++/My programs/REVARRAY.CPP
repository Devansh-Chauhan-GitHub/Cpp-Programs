#include<iostream.h>
#include<conio.h>
#include<string.h>
void main()
{
clrscr();
int i,k=0;
char str[80],word[80];
cout<<"Enter the string : ";
cin.getline(str,80);
strcat(str," ");
for(i=0;str[i]!='\0';i++)
{
if(str[i]!=' ')
{
word[k]=str[i];
k=k+1;
}
else
{
while(k>0)
{
cout<<word[--k];
}
cout<<str[i];
}
}
getch();
}