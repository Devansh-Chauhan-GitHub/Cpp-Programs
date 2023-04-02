#include<iostream.h>
#include<conio.h>
void main()
{
clrscr();
unsigned seconds,minutes,hours,secs_left,mins_left;
cout<<"enter no of seconds<65000:";
cin>>seconds;
hours=seconds/3600;
minutes=seconds%3600;
mins_left=minutes/60;
secs_left=minutes%60;
cout<<"no of seconds entered:"<<seconds;
cout<<endl<<"hours:"<<hours;
cout<<endl<<"minutes:"<<mins_left;
cout<<endl<<"seconds:"<<secs_left;
cout<<endl<<"therefore,"<<seconds<<"seconds means,"<<hours<<"hrs";
cout<<mins_left<<"minutes"<<secs_left<<"seconds";
getch();
}
