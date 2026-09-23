#include <iostream>
using namespace std;

// 1. SOBRECARGA (Tiempo de Compilacion)
class Calculadora {
public:
    int sumar(int a, int b) { return a + b; }
    double sumar(double a, double b) { return a + b; }
};

// 2. POLIMORFISMO (Tiempo de Ejecucion)
class Forma {
public:
    virtual void dibujar() { cout << "Dibujando Forma...\n"; }
};

class Circulo : public Forma {
public:
    void dibujar() override { cout << "Dibujando Circulo...\n"; }
};

int main() {
    // Ejemplo Sobrecarga
    Calculadora calc;
    cout << calc.sumar(5, 10) << endl;
    cout << calc.sumar(3.5, 2.5) << endl;

    // Ejemplo Polimorfismo
    Forma* f = new Circulo();
    f->dibujar(); // Llama a Circulo::dibujar en tiempo de ejecucion
    delete f;

    return 0;
}