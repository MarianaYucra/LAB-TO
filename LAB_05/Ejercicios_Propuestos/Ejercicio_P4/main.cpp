#include <iostream>
#include <string>
using namespace std;

template <typename T>
class ArbolBinario {
    struct Nodo {
        T dato;
        Nodo *izq;
        Nodo *der;
        Nodo(const T &d) : dato(d), izq(nullptr), der(nullptr) {}
    };
    Nodo *raiz = nullptr;

    Nodo *insertarNodo(Nodo *n, const T &v) {
        if (!n) return new Nodo(v);
        if (v < n->dato) n->izq = insertarNodo(n->izq, v);
        else             n->der = insertarNodo(n->der, v);
        return n;
    }
    void inorden(Nodo *n) const {
        if (!n) return;
        inorden(n->izq);
        cout << n->dato << " ";
        inorden(n->der);
    }
    void destruir(Nodo *n) {
        if (!n) return;
        destruir(n->izq);
        destruir(n->der);
        delete n;
    }
public:
    ArbolBinario() = default;
    ArbolBinario(const ArbolBinario &) = delete;
    ArbolBinario &operator=(const ArbolBinario &) = delete;
    ~ArbolBinario() { destruir(raiz); }

    void insertar(const T &v) { raiz = insertarNodo(raiz, v); }
    void mostrar() const { inorden(raiz); cout << endl; }
};

int main()
{
    ArbolBinario<int> enteros;
    for (int v : {50, 30, 70, 20, 40, 60, 80}) enteros.insertar(v);
    enteros.mostrar();              // 20 30 40 50 60 70 80

    ArbolBinario<double> reales;
    for (double v : {3.5, 1.2, 9.9, 0.4}) reales.insertar(v);
    reales.mostrar();               // 0.4 1.2 3.5 9.9

    ArbolBinario<char> letras;
    for (char c : string("arequipa")) letras.insertar(c);
    letras.mostrar();               // a a e i p q r u
    return 0;
}