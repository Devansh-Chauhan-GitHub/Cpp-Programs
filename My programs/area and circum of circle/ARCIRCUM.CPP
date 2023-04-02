#include<iostream.h>
#include<conio.h>
void main()
{
clrscr();
float r,a,c;
cout<<endl<<"enter the radius of the circle : ";
cin>>r;
const float pi=3.14;
a=pi*r*r;
c=2*pi*r;
cout<<endl<<"the area of the circle : "<<a;
cout<<endl<<"circumference of the circle : "<<c;
getch();
}