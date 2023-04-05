#include<iostream>
using namespace std;

int main(){

    // 1.  Sum of Natural numbers
    int num, sum = 0;
    cout << "Enter num: ";
    cin >> num;

    for (int i = 1; i <= num; i++){
        sum += i;
    }
    cout << "Sum is: " << sum << endl;


    // 2. Program to find Factorial
    int fact = 1, num2;
    cout << "Enter number: ";
    cin >> num2;

    for (int i = 0; i < num2; i++){
        fact *= num2--;
    }


    cout << "Factorial is: " << fact << endl;


    // Generate Multiplication Table
    int multi;

    cout << "Enter number: ";
    cin >> multi;

    for (int i = 1; i <= 10; i++){
        cout << multi << " x " << i << " = " << multi*i << endl;
    }

    return 0;
}