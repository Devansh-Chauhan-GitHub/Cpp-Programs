#include<iostream.h>
#include<conio.h>
void main()
{
clrscr();
int n,num,rev=0,d;
cout<<"enter no:- ";
cin>>n;
num=n;
while(n!=0)
{
d=n%10;
rev=rev*10+d;
n=n/10;
}
cout<<"reverse no :- "<<rev;
if(rev==num)
{
cout<<endl<<"palindrome";
}
else
{
cout<<endl<<"not";
}
getch();
}