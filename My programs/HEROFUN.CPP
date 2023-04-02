#include<iostream.h>
#include<conio.h>
#include<math.h>
void res(int,int,int);
void main()
{
clrscr();
int a,b,c;
cout<<"\n\t\tEnter first side : ";
cin>>a;
cout<<"\n\t\tEnter second side : ";
cin>>b;
cout<<"\n\t\tEnter third side : ";
cin>>c;
res(a,b,c);
getch();
}
void res(int a,int b,int c)
{
int s,ar;
s=((a+b+c)/2);
ar=sqrt(s*(s-a)*(s-b)*(s-c));
cout<<"\n\tresult is "<<ar;
}