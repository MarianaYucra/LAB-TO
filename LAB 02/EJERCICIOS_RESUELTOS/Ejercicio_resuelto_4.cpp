#include <iostream>
using namespace std;

int main() {
    int localvariable = 5;
    int *pLocal = &localvariable;
    int *pHeap = new int;

    if (pHeap == NULL) {
        cout << "Error! No memory for pHeap!!";
        return 0;
    }

    *pHeap = 7;
    cout << "localvariable: " << localvariable << "\n";
    cout << "*pLocal: " << *pLocal << "\n";
    cout << "*pHeap: " << *pHeap << "\n";

    delete pHeap;

    pHeap = new int;
    if (pHeap == NULL) {
        cout << "Error! No memory for pHeap!!";
        return 0;
    }

    *pHeap = 9;
    cout << "*pHeap: " << *pHeap << "\n";

    delete pHeap;

    return 0;
}