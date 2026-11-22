#include <iostream>
#include <string>
#include "ToyFactory.h"
using namespace std;

// client
int main() {
    int type;
    while (true) {
        cout << endl << "Ingresa un tipo (1 a 3) o '0' para salir" << endl;
        if (!(cin >> type)) {                       // entrada no numerica
            if (cin.eof()) break;
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Entrada invalida, escribe un numero" << endl;
            continue;
        }
        if (type == 0) break;
        auto v = ToyFactory::createToy(type);
        if (v) v->showProduct();                    // unique_ptr libera el juguete solo
    }
    cout << "Saliendo..." << endl;
    return 0;
}