#include <iostream>
#include <algorithm>
#include <utility>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <numeric>
#include <cmath>
#include <stdexcept>
using namespace std;

// ---------- Utilidades ----------
template <typename T>
T limitar(T v, T lo, T hi) { return v < lo ? lo : (v > hi ? hi : v); }   // plantilla (template)

enum class Moneda { PEN, USD };
string simbolo(Moneda m) { return m == Moneda::PEN ? "S/" : "US$"; }

struct Fecha {
    int a, m, d;
    Fecha(int a_ = 2026, int m_ = 1, int d_ = 1) : a(a_), m(m_), d(d_) {}
    bool operator<=(const Fecha& o) const {
        return a != o.a ? a < o.a : (m != o.m ? m < o.m : d <= o.d);
    }
    string str() const {
        ostringstream os;
        os << setfill('0') << setw(4) << a << '-' << setw(2) << m << '-' << setw(2) << d;
        return os.str();
    }
};

string dinero(double v) { ostringstream os; os << fixed << setprecision(2) << v; return os.str(); }

struct Movimiento { Fecha fecha; string desc; double monto; };   // monto > 0 abono, < 0 cargo
struct RangoTasa  { double min, max; };                          // en % anual

// ---------- Jerarquía de cuentas ----------
class Cuenta {
protected:
    string id_;
    Moneda mon_;
    vector<string> titulares_;
    double saldo_ = 0.0;               // saldo < 0 significa deuda
    double tasa_;                      // % anual efectiva
    vector<Movimiento> movs_;
    void registrar(Fecha f, const string& d, double m) { saldo_ += m; movs_.push_back({f, d, m}); }
public:
    Cuenta(string id, Moneda mon, vector<string> tit, double tasa)
        : id_(move(id)), mon_(mon), titulares_(move(tit)), tasa_(tasa) {}
    virtual ~Cuenta() = default;

    virtual string tipo() const = 0;
    virtual RangoTasa rango() const = 0;

    // Cargo genérico: sin sobregiro. Las tarjetas lo redefinen (polimorfismo)
    virtual bool retirar(double monto, Fecha f, const string& desc) {
        if (monto <= 0 || saldo_ - monto < 0) return false;
        registrar(f, desc, -monto);
        return true;
    }
    void depositar(double monto, Fecha f, const string& desc = "Deposito") {
        if (monto <= 0) throw invalid_argument("El monto debe ser positivo");
        registrar(f, desc, monto);
    }

    double tasaMensual() const { return pow(1 + tasa_ / 100.0, 1.0 / 12.0) - 1; }
    void aplicarInteres(Fecha f) {     // abono si hay saldo, cargo si hay deuda
        double i = saldo_ * tasaMensual();
        if (fabs(i) >= 0.005) registrar(f, i > 0 ? "Interes ganado" : "Interes devengado", i);
    }

    // Sobrecarga: saldo actual y saldo con corte a una fecha
    double saldo() const { return saldo_; }
    double saldoAl(Fecha corte) const {
        return accumulate(movs_.begin(), movs_.end(), 0.0,
            [&](double acc, const Movimiento& m) { return m.fecha <= corte ? acc + m.monto : acc; });
    }

    const string& id() const { return id_; }
    Moneda moneda() const { return mon_; }
    double tasa() const { return tasa_; }
    const vector<string>& titulares() const { return titulares_; }
    const vector<Movimiento>& movimientos() const { return movs_; }
    bool mancomunada() const { return titulares_.size() > 1; }
    virtual bool esDeuda() const { return false; }
};

class CuentaAhorro      : public Cuenta { public: using Cuenta::Cuenta; string tipo() const override { return "Ahorro"; }      RangoTasa rango() const override { return {0.45, 1.00}; } };
class CuentaJoven       : public Cuenta { public: using Cuenta::Cuenta; string tipo() const override { return "Joven"; }       RangoTasa rango() const override { return {1.00, 2.50}; } };
class CuentaPlus        : public Cuenta { public: using Cuenta::Cuenta; string tipo() const override { return "Plus"; }        RangoTasa rango() const override { return {2.50, 4.50}; } };
class CuentaEmpresarial : public Cuenta { public: using Cuenta::Cuenta; string tipo() const override { return "Empresarial"; } RangoTasa rango() const override { return {0.45, 3.00}; } };

class TarjetaCredito : public Cuenta {
    double limite_;
public:
    TarjetaCredito(string id, Moneda mon, vector<string> tit, double tasa, double limite)
        : Cuenta(move(id), mon, move(tit), tasa), limite_(limite) {}
    string tipo() const override { return "Tarjeta de credito"; }
    RangoTasa rango() const override { return {15.00, 26.50}; }
    bool esDeuda() const override { return true; }
    bool retirar(double monto, Fecha f, const string& desc) override {   // compra con tarjeta
        if (monto <= 0 || saldo_ - monto < -limite_) return false;       // excede el limite
        registrar(f, desc, -monto);
        return true;
    }
    double deuda() const { return -saldo_; }
    double disponible() const { return limite_ + saldo_; }
};

class PrestamoPersonal : public Cuenta {
    double monto_;
    int mesesRestantes_;
public:
    PrestamoPersonal(string id, Moneda mon, vector<string> tit, double tasa, double monto, int plazo, Fecha f)
        : Cuenta(move(id), mon, move(tit), tasa), monto_(monto), mesesRestantes_(plazo) {
        registrar(f, "Desembolso del prestamo", -monto);
    }
    string tipo() const override { return "Prestamo personal"; }
    RangoTasa rango() const override { return {8.55, 11.25}; }
    bool esDeuda() const override { return true; }
    bool retirar(double, Fecha, const string&) override { return false; }   // no se retira de un prestamo
    double deuda() const { return -saldo_; }
    int mesesRestantes() const { return mesesRestantes_; }
    double monto() const { return monto_; }
    double cuota() const {             // sistema frances sobre la deuda y los meses que quedan
        double r = tasaMensual();
        return deuda() * r / (1 - pow(1 + r, -mesesRestantes_));
    }
    void cuotaPagada() { if (mesesRestantes_ > 0) --mesesRestantes_; }
};

// ---------- Builder + registro de tipos (la entidad puede crear nuevos) ----------
struct Datos {
    string id;
    Moneda mon = Moneda::PEN;
    vector<string> titulares;
    double tasa = -1;                  // -1 = no indicada
    double limite = 0;                 // limite de tarjeta o monto del prestamo
    int plazo = 12;
    double calificacion = -1;          // 0..100, para prestamos
    Fecha fecha;
};
using Fabrica = function<shared_ptr<Cuenta>(const Datos&)>;

map<string, Fabrica>& registro() { static map<string, Fabrica> r; return r; }
void registrarTipo(const string& nombre, Fabrica f) { registro()[nombre] = move(f); }

class CuentaBuilder {
    string tipo_;
    Datos d_;
public:
    CuentaBuilder& tipo(string t)     { tipo_ = move(t); return *this; }
    CuentaBuilder& id(string i)       { d_.id = move(i); return *this; }
    CuentaBuilder& moneda(Moneda m)   { d_.mon = m; return *this; }
    CuentaBuilder& titular(string t)  { d_.titulares.push_back(move(t)); return *this; }   // 2 o mas = mancomunada
    CuentaBuilder& tasa(double t)     { d_.tasa = t; return *this; }
    CuentaBuilder& limite(double l)   { d_.limite = l; return *this; }
    CuentaBuilder& plazo(int p)       { d_.plazo = p; return *this; }
    CuentaBuilder& calificacion(double c) { d_.calificacion = c; return *this; }
    CuentaBuilder& fecha(Fecha f)     { d_.fecha = f; return *this; }

    shared_ptr<Cuenta> build() {
        auto it = registro().find(tipo_);
        if (it == registro().end()) throw invalid_argument("Tipo de cuenta desconocido: " + tipo_);
        if (d_.titulares.empty()) throw invalid_argument("La cuenta necesita al menos un titular");
        auto c = it->second(d_);
        RangoTasa r = c->rango();
        if (c->tasa() < r.min || c->tasa() > r.max) {
            ostringstream os;
            os << "Tasa " << c->tasa() << "% fuera del rango permitido (" << r.min << "% - " << r.max << "%) para " << c->tipo();
            throw invalid_argument(os.str());
        }
        return c;
    }
};

// ---------- Operaciones del banco ----------
// Comision de transferencia nacional (%), segun moneda y tipo; limitada al rango permitido
double comisionTransferencia(const Cuenta& origen) {
    auto base = [](const Cuenta& c) {
        if (c.moneda() == Moneda::USD) return c.tipo() == "Empresarial" ? 0.21 : 0.35;
        if (c.tipo() == "Plus" || c.tipo() == "Empresarial") return 0.5;
        return c.tipo() == "Joven" ? 1.0 : 2.0;
    };
    return origen.moneda() == Moneda::USD ? limitar(base(origen), 0.21, 0.35)
                                          : limitar(base(origen), 0.5, 4.0);
}

bool transferir(Cuenta& o, Cuenta& d, double monto, Fecha f) {
    if (o.moneda() != d.moneda()) throw invalid_argument("Transferencia entre monedas distintas no permitida");
    double com = monto * comisionTransferencia(o) / 100.0;
    if (!o.retirar(monto + com, f, "Transferencia a " + d.id() + " (incluye comision " + dinero(com) + ")"))
        return false;
    d.depositar(monto, f, "Transferencia de " + o.id());
    return true;
}

// Pago de deuda: sirve para adelantar (monto parcial) o liquidar (monto = deuda total)
bool pagarDeuda(Cuenta& origen, Cuenta& deuda, double monto, Fecha f) {
    if (!deuda.esDeuda()) throw invalid_argument("La cuenta destino no es una deuda");
    if (origen.moneda() != deuda.moneda()) throw invalid_argument("Moneda distinta");
    monto = min(monto, -deuda.saldo());                      // no se paga de mas
    if (monto <= 0 || !origen.retirar(monto, f, "Pago a " + deuda.id())) return false;
    deuda.depositar(monto, f, "Pago recibido de " + origen.id());
    return true;
}
bool liquidarDeuda(Cuenta& origen, Cuenta& deuda, Fecha f) { return pagarDeuda(origen, deuda, -deuda.saldo(), f); }

// ---------- Cliente y estado de cuenta ----------
class Cliente {
    string nombre_;
    vector<shared_ptr<Cuenta>> cuentas_;     // shared_ptr: una cuenta mancomunada la comparten varios clientes
    string estado(Fecha corte, const function<bool(const Cuenta&)>& filtro, const string& titulo) const {
        ostringstream os;
        os << fixed << setprecision(2);
        os << "\n==== ESTADO DE CUENTA (" << titulo << ") de " << nombre_ << " al " << corte.str() << " ====\n";
        map<string, double> totales;                          // total por moneda
        for (const auto& c : cuentas_) {
            if (!filtro(*c)) continue;
            os << "\n[" << c->tipo() << "] " << c->id() << " (" << simbolo(c->moneda()) << ") tasa " << c->tasa() << "%";
            if (c->mancomunada()) {
                os << " MANCOMUNADA: ";
                for (size_t i = 0; i < c->titulares().size(); ++i) os << (i ? " y " : "") << c->titulares()[i];
            }
            os << "\n";
            for (const auto& m : c->movimientos())
                if (m.fecha <= corte)
                    os << "  " << m.fecha.str() << "  " << left << setw(46) << m.desc << right << setw(11) << m.monto << "\n";
            double s = c->saldoAl(corte);
            os << "  Saldo al corte: " << simbolo(c->moneda()) << " " << s << (c->esDeuda() ? "  (deuda)" : "") << "\n";
            totales[simbolo(c->moneda())] += s;
        }
        os << "\nSaldo neto por moneda:";
        for (const auto& kv : totales) os << "  " << kv.first << " " << kv.second;
        os << "\n";
        return os.str();
    }
public:
    explicit Cliente(string n) : nombre_(move(n)) {}
    void agregar(shared_ptr<Cuenta> c) { cuentas_.push_back(move(c)); }
    // Sobrecarga: estado global o filtrado por tipo
    string estadoDeCuenta(Fecha corte) const {
        return estado(corte, [](const Cuenta&) { return true; }, "GLOBAL");
    }
    string estadoDeCuenta(Fecha corte, const string& tipo) const {
        return estado(corte, [&](const Cuenta& c) { return c.tipo() == tipo; }, "TIPO: " + tipo);
    }
};

// Un tipo nuevo que la entidad crea "segun necesidad del mercado"
class CuentaSueldo : public Cuenta {
public:
    using Cuenta::Cuenta;
    string tipo() const override { return "Sueldo"; }
    RangoTasa rango() const override { return {0.45, 1.50}; }
};

int main() {
    // La entidad registra sus productos (cada uno es una lambda que fabrica el objeto)
    registrarTipo("Ahorro",      [](const Datos& d) { return make_shared<CuentaAhorro>(d.id, d.mon, d.titulares, d.tasa); });
    registrarTipo("Joven",       [](const Datos& d) { return make_shared<CuentaJoven>(d.id, d.mon, d.titulares, d.tasa); });
    registrarTipo("Plus",        [](const Datos& d) { return make_shared<CuentaPlus>(d.id, d.mon, d.titulares, d.tasa); });
    registrarTipo("Empresarial", [](const Datos& d) { return make_shared<CuentaEmpresarial>(d.id, d.mon, d.titulares, d.tasa); });
    registrarTipo("Tarjeta",     [](const Datos& d) { return make_shared<TarjetaCredito>(d.id, d.mon, d.titulares, d.tasa, d.limite); });
    registrarTipo("Prestamo",    [](const Datos& d) {
        // tasa sujeta a calificacion: 100 puntos -> 8.55%, 0 puntos -> 11.25%
        double tasa = d.tasa >= 0 ? d.tasa : 11.25 - (limitar(d.calificacion, 0.0, 100.0) / 100.0) * (11.25 - 8.55);
        return make_shared<PrestamoPersonal>(d.id, d.mon, d.titulares, tasa, d.limite, d.plazo, d.fecha);
    });
    registrarTipo("Sueldo",      [](const Datos& d) { return make_shared<CuentaSueldo>(d.id, d.mon, d.titulares, d.tasa); });

    Cliente ana("Ana Perez"), luis("Luis Rojas");

    auto ahorro = CuentaBuilder().tipo("Ahorro").id("AH-001").titular("Ana Perez").tasa(0.80).build();
    auto plus   = CuentaBuilder().tipo("Plus").id("PL-001").titular("Ana Perez").titular("Luis Rojas").tasa(3.50).build();   // mancomunada
    auto joven  = CuentaBuilder().tipo("Joven").id("JV-001").titular("Luis Rojas").tasa(1.80).build();
    auto emp    = CuentaBuilder().tipo("Empresarial").id("EM-001").moneda(Moneda::USD).titular("Ana Perez").tasa(1.20).build();
    auto tc     = CuentaBuilder().tipo("Tarjeta").id("TC-001").titular("Ana Perez").tasa(22.0).limite(5000).build();
    auto prest  = CuentaBuilder().tipo("Prestamo").id("PR-001").titular("Ana Perez").limite(10000).plazo(12)
                                 .calificacion(80).fecha(Fecha(2026, 11, 2)).build();
    ana.agregar(ahorro); ana.agregar(plus); ana.agregar(emp); ana.agregar(tc); ana.agregar(prest);
    luis.agregar(plus);  luis.agregar(joven);

    cout << fixed << setprecision(2);
    cout << "Tasa del prestamo segun calificacion 80: " << prest->tasa() << "%\n";

    // --- Operaciones de noviembre ---
    ahorro->depositar(3000, Fecha(2026, 11, 1), "Deposito inicial");
    plus->depositar(1500, Fecha(2026, 11, 1), "Deposito inicial");
    emp->depositar(2000, Fecha(2026, 11, 1), "Deposito inicial");
    ahorro->depositar(10000, Fecha(2026, 11, 2), "Desembolso prestamo PR-001");     // el prestamo se acredita en la cuenta
    tc->retirar(1200, Fecha(2026, 11, 5), "Compra supermercado");
    tc->retirar(800,  Fecha(2026, 11, 8), "Compra electronica");
    cout << "Compra de 4500 con TC (excede limite): " << (tc->retirar(4500, Fecha(2026, 11, 9), "Compra") ? "aprobada" : "rechazada") << "\n";
    cout << "Comision transferencia desde Ahorro (PEN): " << comisionTransferencia(*ahorro) << "%\n";
    transferir(*ahorro, *plus, 500, Fecha(2026, 11, 10));
    try { transferir(*ahorro, *emp, 100, Fecha(2026, 11, 11)); }
    catch (const invalid_argument& e) { cout << "Error esperado: " << e.what() << "\n"; }

    // Registrar un tipo nuevo en tiempo de ejecucion y fallar con una tasa fuera de rango
    try { CuentaBuilder().tipo("Ahorro").id("AH-002").titular("Ana Perez").tasa(5.0).build(); }
    catch (const invalid_argument& e) { cout << "Error esperado: " << e.what() << "\n"; }
    auto sueldo = CuentaBuilder().tipo("Sueldo").id("SU-001").titular("Ana Perez").tasa(1.0).build();
    ana.agregar(sueldo);
    sueldo->depositar(2500, Fecha(2026, 11, 15), "Abono de sueldo");

    // --- Cierre de mes: intereses ---
    Fecha fin(2026, 11, 30);
    for (auto& c : {ahorro, plus, emp, tc, prest, sueldo}) c->aplicarInteres(fin);

    // --- Diciembre: pagar cuota, adelantar y liquidar ---
    auto* p = dynamic_cast<PrestamoPersonal*>(prest.get());
    cout << "Cuota mensual del prestamo (12 cuotas): " << p->cuota() << "\n";
    pagarDeuda(*ahorro, *prest, p->cuota(), Fecha(2026, 12, 1)); p->cuotaPagada();
    pagarDeuda(*ahorro, *prest, 2000, Fecha(2026, 12, 2));                    // adelanto de capital
    cout << "Cuota despues de adelantar 2000: " << p->cuota() << "\n";
    liquidarDeuda(*ahorro, *tc, Fecha(2026, 12, 3));                           // liquida la tarjeta

    cout << ana.estadoDeCuenta(Fecha(2026, 11, 20));
    cout << ana.estadoDeCuenta(Fecha(2026, 12, 5), "Ahorro");
    cout << luis.estadoDeCuenta(Fecha(2026, 12, 5));
    return 0;
}