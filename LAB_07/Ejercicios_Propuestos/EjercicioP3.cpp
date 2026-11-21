#include <iostream>
#include <utility>
#include <iomanip>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
using namespace std;

// ---------- Lista de componentes: una estructura ----------
struct Componente {
    string nombre;
    string color;
    int cantidad;
    double precioUnit;
    double subtotal() const { return cantidad * precioUnit; }
};

// ---------- Catálogo (opciones a disposición del cliente) ----------
struct Catalogo {
    static const map<string, double>& modelos() {          // precio base
        static const map<string, double> m = {{"Sedan", 18000}, {"SUV", 26000}, {"Deportivo", 38000}};
        return m;
    }
    static const map<string, double>& motorizaciones() {   // recargo
        static const map<string, double> m = {{"1.6 Gasolina", 0}, {"2.0 Turbo", 4500}, {"Electrico", 9000}};
        return m;
    }
    static const map<string, double>& paises() {           // factor sobre el precio (aranceles, costos locales)
        static const map<string, double> m = {{"Japon", 1.00}, {"Alemania", 1.15}, {"Peru", 0.92}};
        return m;
    }
    static const map<string, double>& piezas() {           // precio unitario
        static const map<string, double> m = {{"Puerta", 350}, {"Llanta", 120}, {"Timon", 180}, {"Asiento", 260},
                                              {"Espejo", 90}, {"Vidrio", 140}, {"Faro", 110}};
        return m;
    }
    static const vector<string>& colores() {
        static const vector<string> c = {"Rojo", "Negro", "Blanco", "Azul", "Plata"};
        return c;
    }
};

// ---------- Producto ----------
class Automovil {
public:
    string modelo, motorizacion, pais;
    double precioBase = 0, recargoMotor = 0, factorPais = 1.0;
    vector<Componente> piezas;

    double precioPiezas() const {
        double t = 0;
        for (const auto& p : piezas) t += p.subtotal();
        return t;
    }
    double precioFinal() const { return (precioBase + recargoMotor + precioPiezas()) * factorPais; }

    void mostrar() const {
        cout << fixed << setprecision(2);
        cout << "Modelo: " << modelo << " | Motor: " << motorizacion << " | Pais: " << pais << "\n";
        cout << "  " << left << setw(10) << "Pieza" << setw(8) << "Color" << right << setw(6) << "Cant." << setw(12) << "P.Unit" << setw(12) << "Subtotal" << "\n";
        for (const auto& p : piezas)
            cout << "  " << left << setw(10) << p.nombre << setw(8) << p.color << right << setw(6) << p.cantidad
                 << setw(12) << p.precioUnit << setw(12) << p.subtotal() << "\n";
        cout << "  Base + motor: " << precioBase + recargoMotor << " | Piezas: " << precioPiezas()
             << " | Factor pais: x" << factorPais << "\n";
        cout << "  PRECIO FINAL (US$): " << precioFinal() << "\n";
    }
};

// ---------- Builder ----------
class AutomovilBuilder {
    unique_ptr<Automovil> a_;
    string colorBase_ = "Blanco";
    template <typename V>
    static double buscar(const map<string, V>& cat, const string& clave, const string& que) {
        auto it = cat.find(clave);
        if (it == cat.end()) throw invalid_argument(que + " no disponible: " + clave);
        return it->second;
    }
public:
    AutomovilBuilder() { reset(); }
    void reset() { a_ = make_unique<Automovil>(); colorBase_ = "Blanco"; }

    AutomovilBuilder& modelo(const string& m)       { a_->precioBase = buscar(Catalogo::modelos(), m, "Modelo"); a_->modelo = m; return *this; }
    AutomovilBuilder& motorizacion(const string& m) { a_->recargoMotor = buscar(Catalogo::motorizaciones(), m, "Motorizacion"); a_->motorizacion = m; return *this; }
    AutomovilBuilder& pais(const string& p)         { a_->factorPais = buscar(Catalogo::paises(), p, "Pais"); a_->pais = p; return *this; }
    AutomovilBuilder& colorBase(const string& c) {                       // color por defecto de las piezas que se agreguen luego
        for (const auto& x : Catalogo::colores()) if (x == c) { colorBase_ = c; return *this; }
        throw invalid_argument("Color no disponible: " + c);
    }
    AutomovilBuilder& pieza(const string& nombre, int cantidad, const string& color = "") {
        if (cantidad <= 0) throw invalid_argument("La cantidad debe ser positiva");
        double precio = buscar(Catalogo::piezas(), nombre, "Pieza");
        string c = color.empty() ? colorBase_ : color;
        for (auto& p : a_->piezas)                                      // si ya existe con ese color, suma cantidad
            if (p.nombre == nombre && p.color == c) { p.cantidad += cantidad; return *this; }
        a_->piezas.push_back({nombre, c, cantidad, precio});
        return *this;
    }

    unique_ptr<Automovil> build() {
        auto cuenta = [&](const string& n) { int t = 0; for (const auto& p : a_->piezas) if (p.nombre == n) t += p.cantidad; return t; };
        string error;
        if (a_->modelo.empty() || a_->motorizacion.empty() || a_->pais.empty()) error = "Debe elegir modelo, motorizacion y pais";
        else if (cuenta("Llanta") < 4)  error = "Se necesitan al menos 4 llantas";
        else if (cuenta("Timon") < 1)   error = "Falta el timon";
        else if (cuenta("Asiento") < 2) error = "Se necesitan al menos 2 asientos";
        if (!error.empty()) { reset(); throw logic_error(error); }
        auto r = move(a_);
        reset();
        return r;
    }
};

// ---------- Director: configuraciones predefinidas ----------
class Director {
    AutomovilBuilder& b_;
public:
    explicit Director(AutomovilBuilder& b) : b_(b) {}
    unique_ptr<Automovil> sedanBasico() {
        return b_.modelo("Sedan").motorizacion("1.6 Gasolina").pais("Japon").colorBase("Blanco")
                 .pieza("Puerta", 4).pieza("Llanta", 4).pieza("Timon", 1).pieza("Asiento", 5).build();
    }
    unique_ptr<Automovil> suvCompleto() {
        return b_.modelo("SUV").motorizacion("2.0 Turbo").pais("Alemania").colorBase("Negro")
                 .pieza("Puerta", 4).pieza("Llanta", 4).pieza("Llanta", 1, "Plata").pieza("Timon", 1)
                 .pieza("Asiento", 7).pieza("Espejo", 2).pieza("Vidrio", 6).pieza("Faro", 4).build();
    }
};

// ---------- Cliente interactivo ----------
int elegir(const string& titulo, const vector<string>& opciones) {
    cout << "\n" << titulo << "\n";
    for (size_t i = 0; i < opciones.size(); ++i) cout << "  " << i + 1 << ") " << opciones[i] << "\n";
    cout << "Opcion: ";
    int n = 0;
    if (!(cin >> n) || n < 1 || n > static_cast<int>(opciones.size())) { cin.clear(); cin.ignore(1000, '\n'); n = 1; cout << "(se usa la opcion 1)\n"; }
    return n - 1;
}
vector<string> claves(const map<string, double>& m) { vector<string> v; for (const auto& kv : m) v.push_back(kv.first); return v; }

void clientePersonaliza(AutomovilBuilder& b) {
    auto modelos = claves(Catalogo::modelos()), motores = claves(Catalogo::motorizaciones()), paises = claves(Catalogo::paises());
    b.modelo(modelos[elegir("Modelo:", modelos)]);
    b.motorizacion(motores[elegir("Motorizacion:", motores)]);
    b.pais(paises[elegir("Pais de fabricacion:", paises)]);
    b.colorBase(Catalogo::colores()[elegir("Color de las piezas:", Catalogo::colores())]);
    b.pieza("Puerta", 4).pieza("Llanta", 4).pieza("Timon", 1).pieza("Asiento", 5);   // piezas esenciales
    auto piezas = claves(Catalogo::piezas());
    cout << "\nPiezas adicionales (elige el numero, 0 para terminar):\n";
    for (size_t i = 0; i < piezas.size(); ++i) cout << "  " << i + 1 << ") " << piezas[i] << "\n";
    int n;
    while (cout << "Pieza: ", cin >> n && n != 0) {
        if (n < 1 || n > static_cast<int>(piezas.size())) { cout << "Opcion invalida\n"; continue; }
        int cant = 1;
        cout << "Cantidad: "; cin >> cant;
        try { b.pieza(piezas[n - 1], cant); } catch (const exception& e) { cout << e.what() << "\n"; }
    }
}

int main() {
    AutomovilBuilder builder;
    Director director(builder);

    cout << "=== Sedan basico (Director) ===\n";
    director.sedanBasico()->mostrar();
    cout << "\n=== SUV completo (Director) ===\n";
    director.suvCompleto()->mostrar();

    cout << "\n=== Arme su propio automovil ===";
    clientePersonaliza(builder);
    try {
        cout << "\n=== Su automovil ===\n";
        builder.build()->mostrar();
    } catch (const exception& e) {
        cout << "No se pudo construir: " << e.what() << "\n";
    }
    return 0;
}