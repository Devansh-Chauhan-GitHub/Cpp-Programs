#include <iostream>
using namespace std;

int main(){

    // Normal Loop 
    for (int i = 1; i <=5; ++i){
        cout << i << " ";
    }
    cout << endl;

    for (int i = 1; i <=5; i++){
        cout << i << " ";
    }
    cout << endl;

    // Ranged for loop
    int num_arry[] = {1, 2, 3, 4, 5};

    for (int item : num_arry){
        cout << item << " ";
    }

    cout<<endl;

    return 0;
}