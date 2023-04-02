#include<iostream.h>
#include<conio.h>
void main()
{
clrscr();
char x[80];
int i,v=0;
cout<<"Enter the line : ";
cin.getline(x,80);
for(i=0;x[i]!='\0';++i)
{
switch (x[i])
{
case 'a':
case 'A':
case 'e':
case 'E':
case 'i':
case 'I':
case 'o':
case 'O':
case 'u':
case 'U': ++v;
}
}
cout<<"The no of vowels are : "<<v;
getch();
}