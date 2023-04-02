#include<iostream.h>
#include<conio.h>
#include<math.h>
#include<process.h>
void main()
{
clrscr();
float a,b,res;
int ch;
cout<<endl<<"Enter 2 numbers : "<<endl;
cin>>a;
cout<<endl;
cin>>b;
start:
cout<<endl<<"****************MENU********************"<<endl;
cout<<endl<<"1 for addition"<<endl;
cout<<endl<<"2 for subtraction"<<endl;
cout<<endl<<"3 for multiplication"<<endl;
cout<<endl<<"4 for divison"<<endl;
cout<<endl<<"5 for exit"<<endl;
cout<<endl<<"Enter your choice"<<endl;
cin>>ch;
if(ch==1)
{
res=a+b;
cout<<endl<<"Result is : "<<res<<endl;
}
else if(ch==2)
{
res=a-b;
cout<<endl<<"Result is : "<<res<<endl;
}
else if(ch==3)
{
res=a*b;
cout<<endl<<"Result is : "<<res<<endl;
}
else if(ch==4)
{
res=a/b;
cout<<endl<<"Result is : "<<res<<endl;
}
else if(ch==5)
{
exit(0);
}
goto start;
getch();
}


