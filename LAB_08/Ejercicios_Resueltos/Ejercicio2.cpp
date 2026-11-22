#include <iostream>
#include <memory>
#include <string>

// ---------- Productos abstractos ----------
class AbstractProductoA {
public:
    virtual ~AbstractProductoA() {}
    virtual std::string Funcion1_A() const = 0;
};
class AbstractProductoB {
public:
    virtual ~AbstractProductoB() {}
    virtual std::string Funcion1_B() const = 0;
    virtual std::string Funcion2_B(const AbstractProductoA& colaborador) const = 0;
};

// ---------- Productos concretos: variante 1 ----------
class CProductoA1 : public AbstractProductoA {
public:
    std::string Funcion1_A() const override { return "Producto A1."; }
};
class CProductoB1 : public AbstractProductoB {
public:
    std::string Funcion1_B() const override { return "Producto B1."; }
    std::string Funcion2_B(const AbstractProductoA& colaborador) const override {
        return "B1 con ayuda de " + colaborador.Funcion1_A();
    }
};

// ---------- Productos concretos: variante 2 ----------
class CProductoA2 : public AbstractProductoA {
public:                                              // la guia olvido este public:
    std::string Funcion1_A() const override { return "Producto A2."; }
};
class CProductoB2 : public AbstractProductoB {
public:
    std::string Funcion1_B() const override { return "Producto B2."; }
    std::string Funcion2_B(const AbstractProductoA& colaborador) const override {
        return "B2 con ayuda de " + colaborador.Funcion1_A();
    }
};

// ---------- Fabrica abstracta y fabricas concretas ----------
class AbstractFactory {
public:
    virtual ~AbstractFactory() {}                    // la guia no lo tenia
    virtual std::unique_ptr<AbstractProductoA> CrearProductoA() const = 0;
    virtual std::unique_ptr<AbstractProductoB> CrearProductoB() const = 0;
};
class CFactory1 : public AbstractFactory {
public:
    std::unique_ptr<AbstractProductoA> CrearProductoA() const override { return std::make_unique<CProductoA1>(); }
    std::unique_ptr<AbstractProductoB> CrearProductoB() const override { return std::make_unique<CProductoB1>(); }
};
class CFactory2 : public AbstractFactory {
public:
    std::unique_ptr<AbstractProductoA> CrearProductoA() const override { return std::make_unique<CProductoA2>(); }
    std::unique_ptr<AbstractProductoB> CrearProductoB() const override { return std::make_unique<CProductoB2>(); }
};

// ---------- Cliente: solo conoce las interfaces abstractas ----------
void Cliente(const AbstractFactory& f) {
    auto producto_a = f.CrearProductoA();
    auto producto_b = f.CrearProductoB();
    std::cout << producto_b->Funcion1_B() << "\n";
    std::cout << producto_b->Funcion2_B(*producto_a) << std::endl;
}

int main() {
    std::cout << "Cliente: Tipo 1 ";
    CFactory1 f1;
    Cliente(f1);
    std::cout << std::endl;

    std::cout << "Cliente: Tipo 2 ";
    CFactory2 f2;
    Cliente(f2);
    return 0;
}