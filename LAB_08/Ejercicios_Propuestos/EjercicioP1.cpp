#include <iostream>
#include <iomanip>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
using namespace std;

// ================= Producto abstracto: una receta =================
struct Ingrediente { string nombre; double cantidad; string unidad; };   // cantidad por porcion

class Receta {
    string nombre_, lugar_;
    int minutos_;
    double costoPorcion_;                   // en soles
    vector<Ingrediente> ing_;
public:
    Receta(string nombre, string lugar, int minutos, double costoPorcion, vector<Ingrediente> ing)
        : nombre_(move(nombre)), lugar_(move(lugar)), minutos_(minutos), costoPorcion_(costoPorcion), ing_(move(ing)) {}
    virtual ~Receta() = default;

    const string& nombre() const { return nombre_; }
    const string& lugar() const { return lugar_; }
    double costo(int porciones) const { return costoPorcion_ * porciones; }

    // Linea de produccion (como en el ejercicio a): el orden es fijo, cada receta personaliza sus pasos
    void preparar() const {
        prepararIngredientes();
        cocinar();
        emplatar();
        cout << "  Etiquetando: " << nombre_ << " - " << lugar_ << "\n";
    }
    void mostrar(int porciones) const {
        cout << "=== " << nombre_ << " (" << lugar_ << ") ===\n"
             << "Porciones: " << porciones << " | Tiempo: " << minutos_ << " min | Costo total: S/ "
             << fixed << setprecision(2) << costo(porciones) << defaultfloat << setprecision(6) << "\n"
             << "Ingredientes:\n";
        for (const auto& i : ing_) cout << "  - " << i.nombre << ": " << i.cantidad * porciones << " " << i.unidad << "\n";
        cout << "Preparacion:\n";
        preparar();
        cout << "\n";
    }
protected:
    virtual void prepararIngredientes() const { cout << "  Preparando ingredientes de " << nombre_ << "\n"; }
    virtual void cocinar() const = 0;       // paso obligatorio: lo define cada receta
    virtual void emplatar() const { cout << "  Emplatando " << nombre_ << "\n"; }
};

// ================= Recetas concretas (ASCII para que la consola de Windows no deforme tildes) =================
// --- Arequipa ---
class Ocopa : public Receta { public:
    Ocopa() : Receta("Ocopa", "Arequipa", 30, 6.50, {{"Papa amarilla", 200, "g"}, {"Huacatay", 20, "g"}, {"Queso fresco", 60, "g"}, {"Aji amarillo", 1, "u"}}) {}
    void cocinar() const override { cout << "  Sancochar las papas y licuar huacatay, queso y aji para la salsa\n"; } };
class RocotoRelleno : public Receta { public:
    RocotoRelleno() : Receta("Rocoto relleno", "Arequipa", 90, 12.00, {{"Rocoto", 1, "u"}, {"Carne molida", 120, "g"}, {"Queso paria", 50, "g"}, {"Huevo", 1, "u"}}) {}
    void cocinar() const override { cout << "  Hervir el rocoto, rellenarlo con la carne y hornear con queso\n"; } };
class QuesoHelado : public Receta { public:
    QuesoHelado() : Receta("Queso helado", "Arequipa", 45, 5.00, {{"Leche evaporada", 150, "ml"}, {"Coco rallado", 20, "g"}, {"Azucar", 40, "g"}, {"Canela", 2, "g"}}) {}
    void cocinar() const override { cout << "  Mezclar la leche con el azucar, batir y congelar con canela\n"; } };
// --- Italia ---
class Bruschetta : public Receta { public:
    Bruschetta() : Receta("Bruschetta", "Italia", 15, 4.00, {{"Pan", 60, "g"}, {"Tomate", 80, "g"}, {"Albahaca", 5, "g"}, {"Ajo", 1, "u"}}) {}
    void cocinar() const override { cout << "  Tostar el pan, frotar ajo y cubrir con tomate y albahaca\n"; } };
class PizzaMargarita : public Receta { public:
    PizzaMargarita() : Receta("Pizza Margarita", "Italia", 40, 11.00, {{"Masa", 250, "g"}, {"Mozzarella", 125, "g"}, {"Tomate", 100, "g"}, {"Albahaca", 5, "g"}}) {}
    void cocinar() const override { cout << "  Estirar la masa, cubrir con tomate y mozzarella y hornear a 250 C\n"; } };
class Tiramisu : public Receta { public:
    Tiramisu() : Receta("Tiramisu", "Italia", 30, 8.00, {{"Mascarpone", 100, "g"}, {"Cafe", 60, "ml"}, {"Bizcocho", 50, "g"}, {"Cacao", 5, "g"}}) {}
    void cocinar() const override { cout << "  Mojar el bizcocho en cafe, alternar capas con mascarpone y refrigerar\n"; } };
// --- Japon ---
class Edamame : public Receta { public:
    Edamame() : Receta("Edamame", "Japon", 10, 3.50, {{"Vainas de soya", 150, "g"}, {"Sal gruesa", 3, "g"}}) {}
    void cocinar() const override { cout << "  Hervir las vainas 5 minutos y salar\n"; } };
class Sushi : public Receta { public:
    Sushi() : Receta("Sushi roll", "Japon", 50, 14.00, {{"Arroz", 100, "g"}, {"Alga nori", 1, "u"}, {"Pescado fresco", 80, "g"}, {"Vinagre de arroz", 15, "ml"}}) {}
    void cocinar() const override { cout << "  Sazonar el arroz con vinagre, enrollar con nori y pescado y cortar\n"; } };
class Mochi : public Receta { public:
    Mochi() : Receta("Mochi", "Japon", 35, 4.50, {{"Harina de arroz glutinoso", 60, "g"}, {"Azucar", 25, "g"}, {"Pasta de frijol dulce", 40, "g"}}) {}
    void cocinar() const override { cout << "  Cocer la masa al vapor, amasar y rellenar con pasta dulce\n"; } };

// ================= Factory con registro: agregar una receta NO obliga a tocar la fabrica =================
class RecetaFactory {
    using Creador = function<unique_ptr<Receta>()>;
    static map<int, Creador>& registro() { static map<int, Creador> r; return r; }
public:
    template <typename T>
    static void registrar(int codigo) { registro()[codigo] = [] { return make_unique<T>(); }; }

    static unique_ptr<Receta> crear(int codigo) {
        auto it = registro().find(codigo);
        if (it == registro().end()) return nullptr;
        return it->second();
    }
    static void listar() {
        for (const auto& kv : registro()) {
            auto r = kv.second();
            cout << "  " << kv.first << ") " << left << setw(16) << r->nombre() << right << " [" << r->lugar() << "]\n";
        }
    }
};

// ================= Abstract Factory: un menu completo por lugar =================
class FabricaMenu {
public:
    virtual ~FabricaMenu() = default;
    virtual string lugar() const = 0;
    virtual unique_ptr<Receta> crearEntrada() const = 0;
    virtual unique_ptr<Receta> crearPlatoFondo() const = 0;
    virtual unique_ptr<Receta> crearPostre() const = 0;
};
class MenuArequipa : public FabricaMenu { public:
    string lugar() const override { return "Arequipa"; }
    unique_ptr<Receta> crearEntrada() const override   { return make_unique<Ocopa>(); }
    unique_ptr<Receta> crearPlatoFondo() const override { return make_unique<RocotoRelleno>(); }
    unique_ptr<Receta> crearPostre() const override    { return make_unique<QuesoHelado>(); } };
class MenuItalia : public FabricaMenu { public:
    string lugar() const override { return "Italia"; }
    unique_ptr<Receta> crearEntrada() const override   { return make_unique<Bruschetta>(); }
    unique_ptr<Receta> crearPlatoFondo() const override { return make_unique<PizzaMargarita>(); }
    unique_ptr<Receta> crearPostre() const override    { return make_unique<Tiramisu>(); } };
class MenuJapon : public FabricaMenu { public:
    string lugar() const override { return "Japon"; }
    unique_ptr<Receta> crearEntrada() const override   { return make_unique<Edamame>(); }
    unique_ptr<Receta> crearPlatoFondo() const override { return make_unique<Sushi>(); }
    unique_ptr<Receta> crearPostre() const override    { return make_unique<Mochi>(); } };

// El cliente solo conoce la fabrica abstracta
void servirMenu(const FabricaMenu& f, int porciones) {
    cout << "##### MENU DE " << f.lugar() << " #####\n\n";
    double total = 0;
    for (auto& receta : {f.crearEntrada(), f.crearPlatoFondo(), f.crearPostre()}) {
        receta->mostrar(porciones);
        total += receta->costo(porciones);
    }
    cout << "COSTO TOTAL DEL MENU: S/ " << fixed << setprecision(2) << total << defaultfloat << setprecision(6) << "\n";
}

// ================= Cliente =================
int leerEntero(const string& pregunta) {
    int n;
    cout << pregunta;
    if (!(cin >> n)) {
        if (cin.eof()) return 0;
        cin.clear(); cin.ignore(1000, '\n');
        return -1;
    }
    return n;
}

int main() {
    RecetaFactory::registrar<Ocopa>(1);          RecetaFactory::registrar<RocotoRelleno>(2);  RecetaFactory::registrar<QuesoHelado>(3);
    RecetaFactory::registrar<Bruschetta>(4);     RecetaFactory::registrar<PizzaMargarita>(5); RecetaFactory::registrar<Tiramisu>(6);
    RecetaFactory::registrar<Edamame>(7);        RecetaFactory::registrar<Sushi>(8);          RecetaFactory::registrar<Mochi>(9);

    while (true) {
        cout << "\n1) Una receta   2) Un menu completo por lugar   0) Salir\nOpcion: ";
        int op;
        if (!(cin >> op)) break;
        if (op == 0) break;
        if (op == 1) {
            cout << "Recetas disponibles:\n";
            RecetaFactory::listar();
            int codigo = leerEntero("Codigo de receta: ");
            int porciones = leerEntero("Porciones: ");
            auto r = RecetaFactory::crear(codigo);
            if (!r || porciones <= 0) { cout << "Receta o porciones invalidas\n"; continue; }
            cout << "\n";
            r->mostrar(porciones);
        } else if (op == 2) {
            cout << "Lugares: 1) Arequipa  2) Italia  3) Japon\n";
            int lugar = leerEntero("Lugar: ");
            int porciones = leerEntero("Porciones: ");
            if (porciones <= 0) { cout << "Porciones invalidas\n"; continue; }
            unique_ptr<FabricaMenu> f;
            if (lugar == 1) f = make_unique<MenuArequipa>();
            else if (lugar == 2) f = make_unique<MenuItalia>();
            else if (lugar == 3) f = make_unique<MenuJapon>();
            else { cout << "Lugar invalido\n"; continue; }
            cout << "\n";
            servirMenu(*f, porciones);
        } else {
            cout << "Opcion invalida\n";
        }
    }
    cout << "Saliendo...\n";
    return 0;
}