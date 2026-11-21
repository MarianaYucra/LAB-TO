#include <iostream>
#include <utility>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
using namespace std;

// ================= Componentes (jerarquía) =================
class Item {
    string nombre_, detalle_;
    double precio_;
public:
    Item(string nombre, double precio, string detalle)
        : nombre_(move(nombre)), detalle_(move(detalle)), precio_(precio) {}
    virtual ~Item() = default;
    virtual string categoria() const = 0;
    const string& nombre() const { return nombre_; }
    const string& detalle() const { return detalle_; }
    double precio() const { return precio_; }
};

// Una clase por categoría; "using Item::Item" hereda el constructor
class Procesador  : public Item { public: using Item::Item; string categoria() const override { return "Procesador"; } };
class Ram         : public Item { public: using Item::Item; string categoria() const override { return "Memoria RAM"; } };
class Disco       : public Item { public: using Item::Item; string categoria() const override { return "Disco"; } };
class Pantalla    : public Item { public: using Item::Item; string categoria() const override { return "Monitor"; } };
class ColorEquipo : public Item { public: using Item::Item; string categoria() const override { return "Color"; } };
class Marca       : public Item { public: using Item::Item; string categoria() const override { return "Marca"; } };
class Impresora   : public Item { public: using Item::Item; string categoria() const override { return "Impresora"; } };
class Accesorio   : public Item { public: using Item::Item; string categoria() const override { return "Otros"; } };

// Variantes concretas (precios en soles)
struct IntelI5  : Procesador { IntelI5()  : Procesador("Intel i5",   3000.00, "2 cores, 4 threads, 8 MB cache") {} };
struct IntelI7  : Procesador { IntelI7()  : Procesador("Intel i7",   4700.00, "4 cores, 8 threads, 16 MB cache") {} };
struct Ryzen7   : Procesador { Ryzen7()   : Procesador("AMD Ryzen 7", 4300.00, "8 cores, 16 threads, 16 MB cache") {} };
struct Ram8GB   : Ram { Ram8GB()  : Ram("RAM 8 GB",  220.00, "DDR4 3200 MHz") {} };
struct Ram16GB  : Ram { Ram16GB() : Ram("RAM 16 GB", 400.00, "DDR4 3200 MHz") {} };
struct Ram32GB  : Ram { Ram32GB() : Ram("RAM 32 GB", 780.00, "DDR4 3200 MHz") {} };
struct HDD      : Disco { HDD() : Disco("Hard disk drive",  150.00, "1 TB, 1050 mbits/s") {} };
struct SSD      : Disco { SSD() : Disco("Solid state drive", 270.00, "250 GB, 1800 mbits/s") {} };
struct PantallaNormal : Pantalla { PantallaNormal() : Pantalla("Non-touch screen", 195.00, "15.6 in FHD 1920x1080") {} };
struct PantallaTactil : Pantalla { PantallaTactil() : Pantalla("Touch screen",     320.00, "15.6 in FHD tactil") {} };
struct PantallaPlasma : Pantalla { PantallaPlasma() : Pantalla("Plasma screen",    650.00, "17 in plasma, alto contraste") {} };
struct ColorPlata : ColorEquipo { ColorPlata() : ColorEquipo("Plata", 0.00,  "Aluminio") {} };
struct ColorNegro : ColorEquipo { ColorNegro() : ColorEquipo("Negro", 0.00,  "Mate") {} };
struct ColorAzul  : ColorEquipo { ColorAzul()  : ColorEquipo("Azul",  50.00, "Edicion especial") {} };
struct MarcaLenovo : Marca { MarcaLenovo() : Marca("Lenovo", 0.00,   "Serie XY") {} };
struct MarcaHP     : Marca { MarcaHP()     : Marca("HP",     100.00, "Serie Pavilion") {} };
struct MarcaDell   : Marca { MarcaDell()   : Marca("Dell",   150.00, "Serie Inspiron") {} };
struct ImpresoraLaser     : Impresora { ImpresoraLaser()     : Impresora("Impresora laser",     480.00, "Monocromatica, 30 ppm") {} };
struct ImpresoraInyeccion : Impresora { ImpresoraInyeccion() : Impresora("Impresora inyeccion", 350.00, "Color, multifuncional") {} };
struct Mouse   : Accesorio { Mouse()   : Accesorio("Mouse inalambrico", 45.00,  "Optico 1600 dpi") {} };
struct Maletin : Accesorio { Maletin() : Accesorio("Maletin",           80.00,  "Acolchado 15.6 in") {} };

// ================= Producto =================
class Laptop {
    vector<unique_ptr<Item>> partes_;     // la laptop es dueña de sus partes
public:
    void agregar(unique_ptr<Item> p) { partes_.push_back(move(p)); }
    bool tiene(const string& categoria) const {
        for (const auto& p : partes_) if (p->categoria() == categoria) return true;
        return false;
    }
    double costo() const {
        double total = 0;
        for (const auto& p : partes_) total += p->precio();
        return total;
    }
    void mostrar() const {
        for (const auto& p : partes_)
            cout << "  " << left << setw(12) << p->categoria() << setw(22) << p->nombre()
                 << setw(34) << p->detalle() << right << setw(9) << fixed << setprecision(2) << p->precio() << "\n";
        cout << "  " << left << setw(68) << "TOTAL (S/)" << right << setw(9) << costo() << "\n";
    }
};

// ================= Builder =================
class LaptopBuilder {
    unique_ptr<Laptop> lap_;
public:
    LaptopBuilder() { reset(); }
    void reset() { lap_ = make_unique<Laptop>(); }

    // Paso genérico: cualquier clase derivada de Item
    template <typename T>
    LaptopBuilder& con() { lap_->agregar(make_unique<T>()); return *this; }

    unique_ptr<Laptop> build() {
        for (const char* obligatoria : {"Procesador", "Memoria RAM", "Disco", "Monitor"})
            if (!lap_->tiene(obligatoria)) {
                reset();                               // descarta el armado incompleto
                throw logic_error(string("Falta componente obligatorio: ") + obligatoria);
            }
        unique_ptr<Laptop> resultado = move(lap_);
        reset();                                   // listo para un nuevo equipo
        return resultado;
    }
};

// ================= Director =================
class Director {
    LaptopBuilder& b_;
public:
    explicit Director(LaptopBuilder& b) : b_(b) {}
    unique_ptr<Laptop> oficina() {
        return b_.con<MarcaLenovo>().con<IntelI5>().con<Ram8GB>().con<HDD>()
                 .con<PantallaNormal>().con<ColorPlata>().build();
    }
    unique_ptr<Laptop> gamer() {
        return b_.con<MarcaDell>().con<Ryzen7>().con<Ram32GB>().con<SSD>()
                 .con<PantallaPlasma>().con<ColorNegro>().con<Mouse>().build();
    }
};

void imprimir(const string& titulo, const Laptop& l) {
    cout << "\n=== " << titulo << " ===\n";
    l.mostrar();
}

int main() {
    LaptopBuilder builder;
    Director director(builder);

    imprimir("Oficina (Director)", *director.oficina());
    imprimir("Gamer (Director)", *director.gamer());

    // El cliente arma su propia configuración, eligiendo solo lo que necesita
    auto propia = builder.con<MarcaHP>().con<IntelI7>().con<Ram16GB>().con<SSD>()
                         .con<PantallaTactil>().con<ColorAzul>()
                         .con<ImpresoraInyeccion>().con<Maletin>().build();
    imprimir("Personalizada (Cliente)", *propia);

    // Equipo incompleto: el builder lo rechaza
    try {
        builder.con<IntelI5>().con<Ram8GB>().build();     // falta disco y monitor
    } catch (const logic_error& e) {
        cout << "\nError al construir: " << e.what() << "\n";
    }
    return 0;
}