#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Interfaz del Builder: lista de pasos disponibles
class IBuilder {
public:
    virtual ~IBuilder() {}
    virtual void ProducirParteA() = 0;
    virtual void ProducirParteB() = 0;
    virtual void ProducirParteC() = 0;
};

// Producto: guarda la lista de características asignadas
class Producto1 {
public:
    vector<string> componentes;
    void ListaComp() const {
        cout << "Componentes: ";
        for (size_t i = 0; i < componentes.size(); i++) {
            cout << componentes[i];
            if (i + 1 < componentes.size()) cout << ", ";
        }
        cout << endl;
    }
};

// Builder concreto
class BuilderEspecifico : public IBuilder {
private:
    Producto1* producto;
public:
    BuilderEspecifico() : producto(nullptr) { Reset(); }
    BuilderEspecifico(const BuilderEspecifico&) = delete;
    BuilderEspecifico& operator=(const BuilderEspecifico&) = delete;
    ~BuilderEspecifico() { delete producto; }

    void Reset() {
        delete producto;                 // evita fuga si se llama con un producto a medio armar
        producto = new Producto1();
    }
    void ProducirParteA() override { producto->componentes.push_back("ParteA1"); }
    void ProducirParteB() override { producto->componentes.push_back("ParteB1"); }
    void ProducirParteC() override { producto->componentes.push_back("ParteC1"); }

    Producto1* GetProducto() {           // entrega el producto y se prepara para uno nuevo
        Producto1* resultado = producto;
        producto = new Producto1();
        return resultado;                // quien lo recibe debe hacer delete
    }
};

// Director: conoce "recetas" predefinidas
class Director {
private:
    IBuilder* builder;
public:
    Director() : builder(nullptr) {}
    void set_builder(IBuilder* b) { builder = b; }
    void BuildProductoMin() { builder->ProducirParteA(); }
    void BuildProductoCompleto() {
        builder->ProducirParteA();
        builder->ProducirParteB();
        builder->ProducirParteC();
    }
};

void ClienteCode(Director& director) {
    BuilderEspecifico* builder = new BuilderEspecifico();
    director.set_builder(builder);

    cout << "ProductoBasico:\n";
    director.BuildProductoMin();
    Producto1* p = builder->GetProducto();
    p->ListaComp();
    delete p;

    cout << "ProductoCompleto:\n";
    director.BuildProductoCompleto();
    p = builder->GetProducto();
    p->ListaComp();
    delete p;

    cout << "ProductoPersonalizado (sin Director):\n";   // el cliente usa el builder directamente
    builder->ProducirParteA();
    builder->ProducirParteC();
    p = builder->GetProducto();
    p->ListaComp();
    delete p;

    delete builder;
}

int main() {
    Director* director = new Director();
    ClienteCode(*director);
    delete director;
    return 0;
}