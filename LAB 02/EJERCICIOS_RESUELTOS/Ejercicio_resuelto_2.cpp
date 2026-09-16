#include <iostream>
using namespace std;

typedef unsigned short int USHORT;

int main() {
    USHORT myAge;
    USHORT *pAge = 0;
    myAge = 5;

    cout << "myAge:\t" << myAge << "\n";
    cout << "pAge:\t" << pAge << "\n";

    pAge = &myAge;

    cout << "*pAge:\t" << *pAge << "\n";
    cout << "pAge:\t" << pAge << "\n\n";

    cout << "Asignar nuevo valor al puntero\n";
    *pAge = 7;
    cout << "*pAge:\t" << *pAge << "\n";
    cout << "myAge:\t" << myAge << "\n";
    cout << "pAge:\t" << pAge << "\n\n";

    cout << "Asignar nuevo valor al puntero\n";
    myAge = 9;
    cout << "myAge:\t" << myAge << "\n";
    cout << "*pAge:\t" << *pAge << "\n";
    cout << "pAge:\t" << pAge << "\n";

    return 0;
}