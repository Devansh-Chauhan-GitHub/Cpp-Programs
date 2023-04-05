#include<iostream>
using namespace std;
int main()
{
    int fact=1,i;
    for(i=1;i<=5;i++)
    {
        fact=fact*i;
    }
    cout<<"fact : "<<fact;
    return 0;
}