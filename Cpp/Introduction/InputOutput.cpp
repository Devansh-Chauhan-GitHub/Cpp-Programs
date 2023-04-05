#include <iostream>
using namespace std;

int main(){
    int num1 = 70;
    double num2 = 256.783;
    char ch = 'A';

    cout << num1 << endl;    // print integer
    cout << num2 << endl;    // print double
    cout << "character: " << ch << endl;    // print char
    
    // Single input

    int input;
    cout << "Enter a number:";
    cin >> input; // Taking input
    cout << "The number is " << input;
    
    // Multiple input | cascading
    int a, b;
    cout << "Enter value of a and b: ";
    cin >> a >> b;
    cout << "a: " << a << " b: " << b;

    return 0;
}