#include <iostream>
using namespace std;

int main(){

    // Post Increment
    int num = 5;
    cout << num++ << endl; // Output: 5

    // Pre Increment
    num = 10;
    cout << ++num << endl; // Output: 11

    // Post Decrement
    num = 16;
    cout << num-- << endl; // Output: 16

    // Pre Decrement
    num = 6;
    cout << --num << endl; // Output: 5

    // Relational Operator
    bool output = 3 == 5; // Output: false

    int out = 5 & 6;
    cout << "Binary output: " << out << endl;
    return 0;

}