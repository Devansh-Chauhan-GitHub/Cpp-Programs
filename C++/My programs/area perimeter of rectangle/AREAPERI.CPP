#include<iostream.h>
#include<conio.h>
void main()
{
clrscr();
float l,b;
cout<<endl<<"enter the length of the rectangle : ";
cin>>l;
cout<<endl<<"enter the breadth of the rectangle : ";
cin>>b;
double a;
a=l*b;
cout<<endl<<"the area of the rectangle is : "<<a;
double p;
p=2*(l+b);
cout<<endl<<"the perimeter of the rectangle is : "<<p;
getch();
}