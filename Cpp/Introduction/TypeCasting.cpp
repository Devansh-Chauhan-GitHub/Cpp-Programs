#include <iostream>
using namespace std;

int main(){
    // Type Conversion
    // 1. Implicit Type Conversion

    int num_int = 23;
    double num_double = num_int;

    cout << "num_int: " << num_int << endl;
    cout << "num_double : " << num_double << endl;

    // 2. Explicit Type Conversion

    double double_num = 3.56;
    cout << "double_num = " << double_num << endl;    

    int num_int1 = (int)double_num;
    cout << "num_int1 = " << num_int1 << endl;    

    double num_int2 = int(double_num);
    cout << "double_num = " << num_int2 << endl;    

    char num_char = char(num_int2);
    cout << "num_char = " << num_char << endl;

    int num_char_num = int(num_char);
    cout << "num_char_num = " << num_char_num << endl;

    return 0;
}