#include <iostream>
#include <string>
using namespace std;

template <typename T>
class ListaEnlazada {
    struct Nodo {
        T dato;
        Nodo *sig;
        Nodo(const T &d) : dato(d), sig(nullptr) {}
    };
    Nodo *cabeza = nullptr;
    Nodo *cola = nullptr;
    int tam = 0;
public:
    ListaEnlazada() = default;
    ListaEnlazada(const ListaEnlazada &) = delete;
    ListaEnlazada &operator=(const ListaEnlazada &) = delete;
    ~ListaEnlazada() {
        Nodo *a = cabeza;
        while (a) { Nodo *s = a->sig; delete a; a = s; }
    }
    void insertarFinal(const T &v) {
        Nodo *n = new Nodo(v);
        if (!cabeza) cabeza = cola = n;
        else { cola->sig = n; cola = n; }
        tam++;
    }
    void insertarInicio(const T &v) {
        Nodo *n = new Nodo(v);
        n->sig = cabeza;
        cabeza = n;
        if (!cola) cola = n;
        tam++;
    }
    bool eliminar(const T &v) {
        Nodo *ant = nullptr;
        for (Nodo *a = cabeza; a; ant = a, a = a->sig) {
            if (a->dato == v) {
                if (ant) ant->sig = a->sig; else cabeza = a->sig;
                if (a == cola) cola = ant;
                delete a; tam--;
                return true;
            }
        }
        return false;
    }
    int tamano() const { return tam; }
    void mostrar() const {
        for (Nodo *a = cabeza; a; a = a->sig) cout << a->dato << " -> ";
        cout << "NULL" << endl;
    }
};

int main()
{
    ListaEnlazada<int> edades;
    edades.insertarFinal(18);
    edades.insertarFinal(21);
    edades.insertarFinal(25);
    edades.insertarInicio(15);
    edades.mostrar();               // 15 -> 18 -> 21 -> 25 -> NULL
    edades.eliminar(21);
    edades.mostrar();               // 15 -> 18 -> 25 -> NULL

    ListaEnlazada<string> palabras;
    palabras.insertarFinal("arequipa");
    palabras.insertarFinal("ciudad");
    palabras.insertarFinal("blanca");
    palabras.mostrar();             // arequipa -> ciudad -> blanca -> NULL
    return 0;
}