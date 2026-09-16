#include <iostream>
using namespace std;

class C {
public:
    int x;
    int* p;
    void fun() { 
        cout << "Valor miembro x == " << x << endl; 
    }
    C() { // constructor por defecto
        x = 13;
        p = &x;
    }
};

void f1(C* cp) { // definición de función
    cout << "3 c1.x == " << (*cp).x << endl;
    cout << "4 c1.x == " << cp->x << endl;
    cout << "5 c1.p == " << *(*cp).p << endl;
    cout << "6 c1.p == " << *cp->p << endl;
    (*cp).fun();
    cp->fun();
}

int main() {
    C c1;       // instancia de C
    C* cptr;    // puntero a clase
    cptr = &c1; // asignado al objeto c1

    cout << "1 c1.x == " << c1.x << endl;
    cout << "2 c1.p == " << *c1.p << endl;
    c1.fun();
    f1(cptr);   // puntero se utiliza como argumento de f1

    return 0;
}