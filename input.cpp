#include<iostream>
using namespace std;
int main()
{
    int a;
    string pass;
    char ch;
    do
    {
    cout<<"Enter Password : ";
    cin>>pass;
    if(pass=="mrchauhan")
    {
        cout<<"password is correct";
        break;
    }
    else
    {
        cout<<"Do you want to continue : ";
        cin>>ch;
    }
    }while(ch=='y');
    system("cls");
    cout<<"Enter your Age : ";
    cin>>a;
    cout<<"Your age is : "<<a;
    return 0;
}