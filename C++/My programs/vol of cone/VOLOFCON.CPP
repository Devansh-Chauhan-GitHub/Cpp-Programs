#include<iostream.h>
#include<conio.h>
#include<math.h>
int main();
{
clrscr();
float r,h,v;
const double pi=3.14;
cout<<endl<<"enter the radius";
cin>>r;
cout<<endl<<"enter the height";
cin>>h;
v=1.0/3.0*pi*r*r*h;
cout<<endl<<"volume is "<<v;
getch();
return(0);
}