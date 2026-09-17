#include <iostream>
using namespace std;

struct Nodo {
    int dato;
    Nodo* siguiente;
    Nodo* anterior;
};

class ListaDoble {
private:
    Nodo* cabeza;
    Nodo* cola;
public:
    ListaDoble() {
        cabeza = NULL;
        cola = NULL;
    }

    void insertar(int valor) {
        Nodo* nuevo = new Nodo;
        nuevo->dato = valor;
        nuevo->siguiente = NULL;
        nuevo->anterior = NULL;

        if (cabeza == NULL) {
            cabeza = nuevo;
            cola = nuevo;
        } else {
            cola->siguiente = nuevo;
            nuevo->anterior = cola;
            cola = nuevo;
        }
    }

    void mostrarAdelante() {
        Nodo* temp = cabeza;
        cout << "Lista (inicio a fin): ";
        while (temp != NULL) {
            cout << temp->dato << " ";
            temp = temp->siguiente;
        }
        cout << endl;
    }

    void mostrarAtras() {
        Nodo* temp = cola;
        cout << "Lista (fin a inicio): ";
        while (temp != NULL) {
            cout << temp->dato << " ";
            temp = temp->anterior;
        }
        cout << endl;
    }

    ~ListaDoble() {
        Nodo* temp = cabeza;
        while (temp != NULL) {
            Nodo* aux = temp;
            temp = temp->siguiente;
            delete aux;
        }
    }
};

int main() {
    ListaDoble lista;
    lista.insertar(10);
    lista.insertar(20);
    lista.insertar(30);

    lista.mostrarAdelante();
    lista.mostrarAtras();

    return 0;
}