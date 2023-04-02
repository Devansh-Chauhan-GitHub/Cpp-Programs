#include<iostream.h>
#include<conio.h>
#include<process.h>
void main()
{
clrscr();
int x[10],flag=0,n,num,pos;
int i;
cout<<"Enter total no of elements/++=--";
cin>>n;
if(n>10)
{
cout<<endl<<"Overflow occurs";
exit(0);
}
cout<<endl<<"Enter the elements : ";
for(i=0;i<n;i++)
cin>>x[i];
cout<<endl<<"Enter the element to be searched : ";
cin>>num;
for(i=0;i<n;i++)
if(x[i]==num)
{
flag=1;
pos=i;
break;
}
if(flag==0)
{
cout<<endl<<"element not found";
}
else
cout<<endl<<"element is at position "<<pos+1;
getch();
}