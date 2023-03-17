#include<iostream>
using namespace std;
int main()
{
    int i,j,a=65;
    for(i=0;i<5;i++)
    {
        for(j=0;j<=i;j++)
        {
            cout<<char(a);
        }
    cout<<"\n";
    a=a+1;
    }
    return 0;
}