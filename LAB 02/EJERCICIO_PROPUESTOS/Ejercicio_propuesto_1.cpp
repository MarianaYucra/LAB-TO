#include <iostream>
#include <string>
using namespace std;

// Clase 2: Núcleo que administra las operaciones matemáticas básicas
class AdministradorOperaciones {
public:
    int operar(int a, int b, char op) {
        if (op == '+') return a + b;
        if (op == '-') return a - b;
        if (op == '*') return a * b;
        if (op == '/') return (b != 0) ? a / b : 0;
        return 0;
    }
};

// Clase 1: Analiza la expresión parseando números y operadores
class Analizador {
private:
    AdministradorOperaciones admin;
public:
    int evaluar(int numeros[], char operadores[], int cantOperadores) {
        int resultado = numeros[0];
        for (int i = 0; i < cantOperadores; i++) {
            resultado = admin.operar(resultado, numeros[i + 1], operadores[i]);
        }
        return resultado;
    }
};

// Clase 3: Recibe y procesa la cadena de texto ingresada
class ProcesadorEntrada {
private:
    Analizador analizador;
public:
    void procesar(string expresion) {
        int numeros[6];
        char operadores[5];
        int cantNumeros = 0;
        int cantOperadores = 0;

        int numActual = 0;
        for (size_t i = 0; i < expresion.length(); i++) {
            char c = expresion[i];
            if (c >= '0' && c <= '9') {
                numActual = numActual * 10 + (c - '0');
            } else if (c == '+' || c == '-' || c == '*' || c == '/') {
                if (cantNumeros < 6) {
                    numeros[cantNumeros++] = numActual;
                    numActual = 0;
                }
                if (cantOperadores < 5) {
                    operadores[cantOperadores++] = c;
                }
            }
        }
        if (cantNumeros < 6) {
            numeros[cantNumeros++] = numActual;
        }

        int resultado = analizador.evaluar(numeros, operadores, cantOperadores);
        cout << "Resultado de \"" << expresion << "\": " << resultado << endl;
    }
};

int main() {
    ProcesadorEntrada p;
    p.procesar("10+37");
    p.procesar("45+14-42");
    p.procesar("1+2+3+4+5+6");
    return 0;
}