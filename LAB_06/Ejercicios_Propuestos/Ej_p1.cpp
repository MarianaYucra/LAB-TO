#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <functional>   // para std::ref
#include <random>
#include <chrono>

struct Nodo {
    int valor;
    Nodo* sig;
    explicit Nodo(int v) : valor(v), sig(nullptr) {}
};

class ListaSegura {
    Nodo* cabeza = nullptr;
    mutable std::mutex mtx;               // protege TODA la lista
public:
    ListaSegura() = default;
    ListaSegura(const ListaSegura&) = delete;
    ListaSegura& operator=(const ListaSegura&) = delete;
    ~ListaSegura() {
        while (cabeza) { Nodo* t = cabeza; cabeza = cabeza->sig; delete t; }
    }
    void insertar(int v) {
        std::lock_guard<std::mutex> lk(mtx);
        Nodo* n = new Nodo(v);
        n->sig = cabeza;
        cabeza = n;
    }
    bool eliminar(int v) {
        std::lock_guard<std::mutex> lk(mtx);
        Nodo* ant = nullptr;
        Nodo* act = cabeza;
        while (act && act->valor != v) { ant = act; act = act->sig; }
        if (!act) return false;
        if (ant) ant->sig = act->sig; else cabeza = act->sig;
        delete act;
        return true;
    }
    bool buscar(int v) const {
        std::lock_guard<std::mutex> lk(mtx);
        for (Nodo* p = cabeza; p; p = p->sig)
            if (p->valor == v) return true;
        return false;
    }
    bool modificar(int v, int nuevo) {
        std::lock_guard<std::mutex> lk(mtx);
        for (Nodo* p = cabeza; p; p = p->sig)
            if (p->valor == v) { p->valor = nuevo; return true; }
        return false;
    }
    void imprimir() const {
        std::lock_guard<std::mutex> lk(mtx);
        for (Nodo* p = cabeza; p; p = p->sig) std::cout << p->valor << " -> ";
        std::cout << "NULL\n";
    }
};

std::mutex mtxCout;                       // evita que se mezclen los mensajes
void log(const std::string& s) {
    std::lock_guard<std::mutex> lk(mtxCout);
    std::cout << s << '\n';
}

int aleatorio(std::mt19937& g, int a, int b) {
    return std::uniform_int_distribution<int>(a, b)(g);
}
void pausa() { std::this_thread::sleep_for(std::chrono::milliseconds(50)); }

void tareaEliminar(ListaSegura& l) {
    std::mt19937 g(std::random_device{}());
    for (int i = 0; i < 10; ++i) {
        int n = aleatorio(g, 1, 20);
        if (l.eliminar(n)) log("[T1] Eliminando: " + std::to_string(n));
        else               log("[T1] No se eliminó " + std::to_string(n));
        pausa();
    }
}
void tareaInsertar(ListaSegura& l) {
    std::mt19937 g(std::random_device{}());
    for (int i = 0; i < 10; ++i) {
        int n = aleatorio(g, 1, 20);
        l.insertar(n);
        log("[T2] Insertando: " + std::to_string(n));
        pausa();
    }
}
void tareaBuscar(ListaSegura& l) {
    std::mt19937 g(std::random_device{}());
    for (int i = 0; i < 10; ++i) {
        int n = aleatorio(g, 1, 20);
        if (l.buscar(n)) log("[T3] Buscado: " + std::to_string(n));
        else             log("[T3] No encontrado: " + std::to_string(n));
        pausa();
    }
}
void tareaModificar(ListaSegura& l) {
    std::mt19937 g(std::random_device{}());
    for (int i = 0; i < 10; ++i) {
        int n = aleatorio(g, 1, 20);
        int suma = aleatorio(g, 1, 10);
        if (l.modificar(n, n + suma))
            log("[T4] Modificando " + std::to_string(n) + " a " + std::to_string(n + suma));
        else
            log("[T4] No se encontró " + std::to_string(n));
        pausa();
    }
}

int main() {
    ListaSegura lista;
    for (int v : {3, 7, 12, 15, 18}) lista.insertar(v);   // datos iniciales
    std::cout << "Lista inicial: "; lista.imprimir();

    std::thread t1(tareaEliminar,  std::ref(lista));
    std::thread t2(tareaInsertar,  std::ref(lista));
    std::thread t3(tareaBuscar,    std::ref(lista));
    std::thread t4(tareaModificar, std::ref(lista));
    t1.join(); t2.join(); t3.join(); t4.join();          // join DESPUÉS de crear todos

    std::cout << "Lista final:   "; lista.imprimir();
    return 0;
}