#include<iostream>
using namespace std;
int main()
{
    int n,i,s=0;
    system("cls");
    cout<<"Enter the no : ";
    cin>>n;
    for(i=1;i<=n;i++)
    {
        s=s+i;
    }
    cout<<"The sum of natural no : "<<s;
    cout<<"\n";
    system("pause");
    return 0;
}