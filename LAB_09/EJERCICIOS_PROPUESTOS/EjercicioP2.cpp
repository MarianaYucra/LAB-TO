#include <iostream>
#include <iomanip>
#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
using namespace std;

enum class Sector { Trepadoras, ArbolesNormales, Ornamentales, Bonsais };
string nombreSector(Sector s) {
    switch (s) {
    case Sector::Trepadoras:      return "Trepadoras";
    case Sector::ArbolesNormales: return "Arboles normales";
    case Sector::Ornamentales:    return "Ornamentales";
    case Sector::Bonsais:         return "Bonsais";
    }
    return "?";
}

// Objeto asociado a cada planta: como se guarda por puntero, el clon debe COPIARLO (copia profunda)
struct Cuidados {
    int riegoCadaDias;
    string luz;
    string nota;
};

// ================= Prototipo abstracto =================
class Planta {
protected:
    string nombre_, cientifico_;
    double altura_;                          // metros
    double precio_;                          // soles
    unique_ptr<Cuidados> cuidados_;
public:
    Planta(string nombre, string cientifico, double altura, double precio, Cuidados c)
        : nombre_(move(nombre)), cientifico_(move(cientifico)), altura_(altura), precio_(precio),
          cuidados_(make_unique<Cuidados>(move(c))) {}
    Planta(const Planta& o)                  // constructor de copia PROFUNDA: cada clon tiene sus propios Cuidados
        : nombre_(o.nombre_), cientifico_(o.cientifico_), altura_(o.altura_), precio_(o.precio_),
          cuidados_(make_unique<Cuidados>(*o.cuidados_)) {}
    Planta& operator=(const Planta&) = delete;
    virtual ~Planta() = default;

    virtual unique_ptr<Planta> clone() const = 0;     // el clon conserva el tipo concreto
    virtual Sector sector() const = 0;
    virtual string atributos() const = 0;             // atributos propios de cada tipo de planta

    const string& nombre() const { return nombre_; }
    double precio() const { return precio_; }
    Cuidados& cuidados() { return *cuidados_; }
    const Cuidados& cuidados() const { return *cuidados_; }
    void setNombre(string n) { nombre_ = move(n); }
    void setAltura(double a) { altura_ = a; }
    void setPrecio(double p) { precio_ = p; }

    void mostrar() const {
        cout << "  - " << left << setw(14) << nombre_ << right << " | " << cientifico_ << " | " << altura_ << " m | S/ "
             << fixed << setprecision(2) << precio_ << defaultfloat << setprecision(6) << " | " << atributos()
             << " | riego cada " << cuidados_->riegoCadaDias << " dias, luz " << cuidados_->luz << "\n";
    }
};

// ================= Prototipos concretos =================
class Trepadora : public Planta {
    string soporte_;
public:
    Trepadora(string n, string c, double alt, double precio, Cuidados cu, string soporte)
        : Planta(move(n), move(c), alt, precio, move(cu)), soporte_(move(soporte)) {}
    unique_ptr<Planta> clone() const override { return make_unique<Trepadora>(*this); }
    Sector sector() const override { return Sector::Trepadoras; }
    string atributos() const override { return "soporte: " + soporte_; }
};
class ArbolNormal : public Planta {
    double alturaAdulta_;
public:
    ArbolNormal(string n, string c, double alt, double precio, Cuidados cu, double adulta)
        : Planta(move(n), move(c), alt, precio, move(cu)), alturaAdulta_(adulta) {}
    unique_ptr<Planta> clone() const override { return make_unique<ArbolNormal>(*this); }
    Sector sector() const override { return Sector::ArbolesNormales; }
    string atributos() const override { return "altura adulta: " + to_string(static_cast<int>(alturaAdulta_)) + " m"; }
};
class Ornamental : public Planta {
    string colorFlor_;
public:
    Ornamental(string n, string c, double alt, double precio, Cuidados cu, string color)
        : Planta(move(n), move(c), alt, precio, move(cu)), colorFlor_(move(color)) {}
    unique_ptr<Planta> clone() const override { return make_unique<Ornamental>(*this); }
    Sector sector() const override { return Sector::Ornamentales; }
    string atributos() const override { return "flor " + colorFlor_; }
};
class Bonsai : public Planta {
    string estilo_;
    int edad_;
public:
    Bonsai(string n, string c, double alt, double precio, Cuidados cu, string estilo, int edad)
        : Planta(move(n), move(c), alt, precio, move(cu)), estilo_(move(estilo)), edad_(edad) {}
    unique_ptr<Planta> clone() const override { return make_unique<Bonsai>(*this); }
    Sector sector() const override { return Sector::Bonsais; }
    string atributos() const override { return "estilo " + estilo_ + ", " + to_string(edad_) + " anios"; }
};

// ================= Fabrica de prototipos (catalogo) =================
class CatalogoPrototipos {
    vector<pair<string, unique_ptr<Planta>>> prototipos_;      // en orden de registro
public:
    void registrar(const string& clave, unique_ptr<Planta> p) { prototipos_.emplace_back(clave, move(p)); }
    size_t cantidad() const { return prototipos_.size(); }
    const string& clave(size_t i) const { return prototipos_.at(i).first; }
    const Planta& prototipo(size_t i) const { return *prototipos_.at(i).second; }
    unique_ptr<Planta> crear(size_t i) const { return prototipos_.at(i).second->clone(); }   // clona, no construye desde cero
};

// ================= Vivero: agrupa las plantas por sector =================
class Vivero {
    map<Sector, vector<unique_ptr<Planta>>> sectores_;
public:
    Vivero() {                                        // los 4 sectores existen desde el inicio
        for (Sector s : {Sector::Trepadoras, Sector::ArbolesNormales, Sector::Ornamentales, Sector::Bonsais}) sectores_[s];
    }
    void agregar(unique_ptr<Planta> p) { Sector s = p->sector(); sectores_[s].push_back(move(p)); }   // va al sector que le corresponde
    void mostrar() const {
        double total = 0;
        for (const auto& kv : sectores_) {
            double valor = 0;
            for (const auto& p : kv.second) valor += p->precio();
            cout << "SECTOR " << nombreSector(kv.first) << " (" << kv.second.size() << " plantas, S/ " << fixed << setprecision(2) << valor << defaultfloat << setprecision(6) << ")\n";
            for (const auto& p : kv.second) p->mostrar();
            total += valor;
        }
        cout << "VALOR TOTAL DEL VIVERO: S/ " << fixed << setprecision(2) << total << defaultfloat << setprecision(6) << "\n";
    }
};

int leerEntero(const string& pregunta) {
    cout << pregunta;
    int n;
    if (!(cin >> n)) { if (cin.eof()) return 0; cin.clear(); cin.ignore(1000, '\n'); return -1; }
    return n;
}

int main() {
    CatalogoPrototipos catalogo;
    catalogo.registrar("buganvilia", make_unique<Trepadora>("Buganvilia", "Bougainvillea glabra", 3.0, 25, Cuidados{3, "sol pleno", "podar tras la floracion"}, "enrejado"));
    catalogo.registrar("hiedra",     make_unique<Trepadora>("Hiedra", "Hedera helix", 2.0, 18, Cuidados{4, "sombra parcial", "crece sola sobre muros"}, "pared"));
    catalogo.registrar("molle",      make_unique<ArbolNormal>("Molle", "Schinus molle", 1.5, 30, Cuidados{7, "sol pleno", "resiste la sequia"}, 12));
    catalogo.registrar("quenua",     make_unique<ArbolNormal>("Quenua", "Polylepis rugulosa", 1.0, 40, Cuidados{6, "sol pleno", "arbol andino de altura"}, 8));
    catalogo.registrar("rosa",       make_unique<Ornamental>("Rosa", "Rosa spp.", 0.6, 12, Cuidados{2, "sol pleno", "retirar flores marchitas"}, "roja"));
    catalogo.registrar("geranio",    make_unique<Ornamental>("Geranio", "Pelargonium", 0.4, 8, Cuidados{3, "sol parcial", "evitar el exceso de agua"}, "rosada"));
    catalogo.registrar("bonsai_ficus", make_unique<Bonsai>("Bonsai Ficus", "Ficus retusa", 0.3, 120, Cuidados{2, "indirecta", "pinzar brotes nuevos"}, "vertical", 5));
    catalogo.registrar("bonsai_pino",  make_unique<Bonsai>("Bonsai Pino", "Pinus thunbergii", 0.35, 200, Cuidados{3, "sol pleno", "alambrar con cuidado"}, "informal", 8));

    Vivero vivero;
    while (true) {
        cout << "\n1) Ver catalogo   2) Agregar plantas   3) Ver vivero por sector   4) Prueba de clonacion profunda   0) Salir\nOpcion: ";
        int op;
        if (!(cin >> op) || op == 0) break;
        if (op == 1) {
            for (size_t i = 0; i < catalogo.cantidad(); ++i) { cout << "  " << i + 1 << ")"; catalogo.prototipo(i).mostrar(); }
        } else if (op == 2) {
            int n = leerEntero("Numero de planta del catalogo: ");
            int cantidad = leerEntero("Cantidad: ");
            if (n < 1 || n > static_cast<int>(catalogo.cantidad()) || cantidad <= 0) { cout << "Seleccion invalida\n"; continue; }
            double altura = 0;
            cout << "Altura en m (0 para la del prototipo): ";
            if (!(cin >> altura)) { cin.clear(); cin.ignore(1000, '\n'); altura = 0; }
            for (int k = 0; k < cantidad; ++k) {
                auto planta = catalogo.crear(n - 1);                    // clon rapido del prototipo
                if (altura > 0) planta->setAltura(altura);              // se personalizan solo los atributos que cambian
                vivero.agregar(move(planta));
            }
            cout << "Agregadas: " << cantidad << " x " << catalogo.clave(n - 1) << "\n";
        } else if (op == 3) {
            vivero.mostrar();
        } else if (op == 4) {
            const Planta& proto = catalogo.prototipo(4);                // la rosa
            auto clon = proto.clone();
            clon->cuidados().riegoCadaDias = 99;                        // se modifica SOLO el clon
            clon->cuidados().nota = "nota propia del clon";
            cout << "Riego del prototipo: " << proto.cuidados().riegoCadaDias << " dias | del clon: " << clon->cuidados().riegoCadaDias << " dias\n";
            cout << "Nota del prototipo: " << proto.cuidados().nota << " | del clon: " << clon->cuidados().nota << "\n";
            cout << "Objetos Cuidados distintos (copia profunda): " << (&proto.cuidados() != &clon->cuidados() ? "si" : "no") << "\n";
        } else {
            cout << "Opcion invalida\n";
        }
    }
    cout << "Saliendo...\n";
    return 0;
}