#include <iostream>
#include <memory>
using namespace std;

class Recurso {
public:
    Recurso() { cout << "Recurso asignado.\n"; }
    ~Recurso() { cout << "Recurso liberado automáticamente.\n"; }
    void usar() { cout << "Usando recurso...\n"; }
};

int main() {
    // Uso de unique_ptr
    unique_ptr<Recurso> uPtr = make_unique<Recurso>();
    uPtr->usar();

    // Uso de shared_ptr
    shared_ptr<Recurso> sPtr1 = make_shared<Recurso>();
    {
        shared_ptr<Recurso> sPtr2 = sPtr1; // Comparte propiedad
        cout << "Contador de referencias: " << sPtr1.use_count() << endl; // Imprime 2
    }
    cout << "Contador de referencias: " << sPtr1.use_count() << endl; // Imprime 1

    return 0; // Se libera automáticamente toda la memoria al salir del scope
}