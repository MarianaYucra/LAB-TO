#include <iostream>
#include <string>
using namespace std;

class NodoBase {
public:
    virtual int evaluar() = 0;
    virtual ~NodoBase() {}
};

class NodoNumero : public NodoBase {
private:
    int valor;
public:
    NodoNumero(int v) : valor(v) {}
    int evaluar() override {
        return valor;
    }
};

class NodoOperacion : public NodoBase {
private:
    char operador;
    NodoBase* izquierdo;
    NodoBase* derecho;
public:
    NodoOperacion(char op, NodoBase* izq, NodoBase* der) 
        : operador(op), izquierdo(izq), derecho(der) {}

    int evaluar() override {
        int izqVal = izquierdo->evaluar();
        int derVal = derecho->evaluar();

        if (operador == '+') return izqVal + derVal;
        if (operador == '*') return izqVal * derVal;
        return 0;
    }

    ~NodoOperacion() {
        delete izquierdo;
        delete derecho;
    }
};

int main() {
    // Construccion manual del arbol segun la figura del laboratorio:
    // Expresion: 54 + ( (5 * 34) + (1 * 2) )
    
    NodoBase* n54 = new NodoNumero(54);
    NodoBase* n5 = new NodoNumero(5);
    NodoBase* n34 = new NodoNumero(34);
    NodoBase* n1 = new NodoNumero(1);
    NodoBase* n2 = new NodoNumero(2);

    NodoBase* prod1 = new NodoOperacion('*', n5, n34); // 5 * 34 = 170
    NodoBase* prod2 = new NodoOperacion('*', n1, n2);  // 1 * 2 = 2
    NodoBase* sumaDer = new NodoOperacion('+', prod1, prod2); // 170 + 2 = 172
    NodoBase* raiz = new NodoOperacion('+', n54, sumaDer);   // 54 + 172 = 226

    cout << "Resultado de la expresion: " << raiz->evaluar() << endl;

    delete raiz; // Libera recursivamente toda la memoria dinamica
    return 0;
}