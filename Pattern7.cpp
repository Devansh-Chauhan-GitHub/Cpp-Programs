#include<iostream>
using namespace std;
int main()
{
    int i,j,k,l;
    for(i=0;i<5;i++)
    {
        for(j=5;j>i;j--)
        {
            cout<<" ";
        }
        for(k=0;k<=i;k++)
        {
            cout<<"*";
            
        }
        for(l=0;l<i;l++)
        {
            cout<<"*";
        }
    cout<<"\n";
    }
    return 0;
}