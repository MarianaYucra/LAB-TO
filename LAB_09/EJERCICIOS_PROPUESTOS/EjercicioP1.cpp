#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <numeric>
#include <cmath>
#include <mutex>
#include <thread>
#include <stdexcept>
using namespace std;

// ---------- Utilidades ----------
template <typename T>
T limitar(T v, T lo, T hi) { return v < lo ? lo : (v > hi ? hi : v); }

struct Fecha {
    int a, m, d;
    Fecha(int a_ = 2026, int m_ = 1, int d_ = 1) : a(a_), m(m_), d(d_) {}
    bool operator<=(const Fecha& o) const { return a != o.a ? a < o.a : (m != o.m ? m < o.m : d <= o.d); }
    string str() const { ostringstream os; os << setfill('0') << setw(4) << a << '-' << setw(2) << m << '-' << setw(2) << d; return os.str(); }
};
string dinero(double v) { ostringstream os; os << fixed << setprecision(2) << v; return os.str(); }

struct Movimiento { Fecha fecha; string desc; double monto; };   // monto > 0 abono, < 0 cargo
struct RangoTasa  { double min, max; };                          // en % anual

// ================= Jerarquia de cuentas =================
class Cuenta {
protected:
    string id_;
    vector<string> titulares_;
    double saldo_ = 0.0;               // saldo < 0 significa deuda
    double tasa_;                      // % anual efectiva
    vector<Movimiento> movs_;
    void registrar(Fecha f, const string& d, double m) { saldo_ += m; movs_.push_back({f, d, m}); }
public:
    Cuenta(string id, vector<string> tit, double tasa) : id_(move(id)), titulares_(move(tit)), tasa_(tasa) {}
    virtual ~Cuenta() = default;
    virtual string tipo() const = 0;
    virtual RangoTasa rango() const = 0;
    virtual bool esDeuda() const { return false; }

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
    void aplicarInteres(Fecha f) {
        double i = saldo_ * tasaMensual();
        if (fabs(i) >= 0.005) registrar(f, i > 0 ? "Interes ganado" : "Interes devengado", i);
    }
    double saldo() const { return saldo_; }
    double saldoAl(Fecha corte) const {
        return accumulate(movs_.begin(), movs_.end(), 0.0,
            [&](double acc, const Movimiento& m) { return m.fecha <= corte ? acc + m.monto : acc; });
    }
    const string& id() const { return id_; }
    double tasa() const { return tasa_; }
    const vector<string>& titulares() const { return titulares_; }
    const vector<Movimiento>& movimientos() const { return movs_; }
    bool mancomunada() const { return titulares_.size() > 1; }
    bool esTitular(const string& c) const { return find(titulares_.begin(), titulares_.end(), c) != titulares_.end(); }
};

class CuentaAhorro      : public Cuenta { public: using Cuenta::Cuenta; string tipo() const override { return "Ahorro"; }      RangoTasa rango() const override { return {0.5, 1.0}; } };
class CuentaJoven       : public Cuenta { public: using Cuenta::Cuenta; string tipo() const override { return "Joven"; }       RangoTasa rango() const override { return {1.0, 2.5}; } };
class CuentaPlus        : public Cuenta { public: using Cuenta::Cuenta; string tipo() const override { return "Plus"; }        RangoTasa rango() const override { return {2.5, 4.5}; } };
class CuentaEmpresarial : public Cuenta { public: using Cuenta::Cuenta; string tipo() const override { return "Empresarial"; } RangoTasa rango() const override { return {0.5, 3.0}; } };

class TarjetaCredito : public Cuenta {
    double limite_;
public:
    TarjetaCredito(string id, vector<string> tit, double tasa, double limite) : Cuenta(move(id), move(tit), tasa), limite_(limite) {}
    string tipo() const override { return "Tarjeta de credito"; }
    RangoTasa rango() const override { return {15.0, 25.0}; }
    bool esDeuda() const override { return true; }
    bool retirar(double monto, Fecha f, const string& desc) override {       // compra con tarjeta
        if (monto <= 0 || saldo_ - monto < -limite_) return false;
        registrar(f, desc, -monto);
        return true;
    }
};

class PrestamoPersonal : public Cuenta {
    int mesesRestantes_;
public:
    PrestamoPersonal(string id, vector<string> tit, double tasa, double monto, int plazo, Fecha f)
        : Cuenta(move(id), move(tit), tasa), mesesRestantes_(plazo) { registrar(f, "Desembolso del prestamo", -monto); }
    string tipo() const override { return "Prestamo personal"; }
    RangoTasa rango() const override { return {8.0, 10.0}; }
    bool esDeuda() const override { return true; }
    bool retirar(double, Fecha, const string&) override { return false; }
    double cuota() const {
        double r = tasaMensual();
        return -saldo_ * r / (1 - pow(1 + r, -mesesRestantes_));
    }
    void cuotaPagada() { if (mesesRestantes_ > 0) --mesesRestantes_; }
};

// ================= SINGLETON: el repositorio de cuentas =================
struct Solicitud {
    string tipo, id;
    vector<string> titulares;
    double tasa, limite, calificacion;
    int plazo;
    Fecha fecha;
    Solicitud(string t, string i, vector<string> tit = {}, double tasa_ = -1, double limite_ = 0,
              double calif = -1, int plazo_ = 12, Fecha f = Fecha())
        : tipo(move(t)), id(move(i)), titulares(move(tit)), tasa(tasa_), limite(limite_), calificacion(calif), plazo(plazo_), fecha(f) {}
};
using Fabrica = function<shared_ptr<Cuenta>(const Solicitud&)>;

class RepositorioCuentas {
    map<string, shared_ptr<Cuenta>> cuentas_;
    map<string, Fabrica> tipos_;
    mutable mutex mtx_;

    RepositorioCuentas() {                               // constructor PRIVADO: nadie mas puede crear un repositorio
        tipos_["Ahorro"]      = [](const Solicitud& s) { return make_shared<CuentaAhorro>(s.id, s.titulares, s.tasa); };
        tipos_["Joven"]       = [](const Solicitud& s) { return make_shared<CuentaJoven>(s.id, s.titulares, s.tasa); };
        tipos_["Plus"]        = [](const Solicitud& s) { return make_shared<CuentaPlus>(s.id, s.titulares, s.tasa); };
        tipos_["Empresarial"] = [](const Solicitud& s) { return make_shared<CuentaEmpresarial>(s.id, s.titulares, s.tasa); };
        tipos_["Tarjeta"]     = [](const Solicitud& s) { return make_shared<TarjetaCredito>(s.id, s.titulares, s.tasa, s.limite); };
        tipos_["Prestamo"]    = [](const Solicitud& s) {
            // tasa sujeta a calificacion: 100 puntos -> 8%, 0 puntos -> 10%
            double tasa = s.tasa >= 0 ? s.tasa : 10.0 - (limitar(s.calificacion, 0.0, 100.0) / 100.0) * 2.0;
            return make_shared<PrestamoPersonal>(s.id, s.titulares, tasa, s.limite, s.plazo, s.fecha);
        };
    }
public:
    RepositorioCuentas(const RepositorioCuentas&) = delete;              // no se puede copiar ni asignar
    RepositorioCuentas& operator=(const RepositorioCuentas&) = delete;

    static RepositorioCuentas& instancia() {             // Singleton de Meyers: unica instancia, segura entre hilos desde C++11
        static RepositorioCuentas repo;
        return repo;
    }

    void registrarTipo(const string& nombre, Fabrica f) {                // la entidad crea productos nuevos
        lock_guard<mutex> lk(mtx_);
        tipos_[nombre] = move(f);
    }
    shared_ptr<Cuenta> abrir(const Solicitud& s) {
        lock_guard<mutex> lk(mtx_);
        auto it = tipos_.find(s.tipo);
        if (it == tipos_.end()) throw invalid_argument("Tipo de cuenta desconocido: " + s.tipo);
        if (s.titulares.empty()) throw invalid_argument("La cuenta necesita al menos un titular");
        if (cuentas_.count(s.id)) throw invalid_argument("Ya existe la cuenta " + s.id);
        auto c = it->second(s);
        RangoTasa r = c->rango();
        if (c->tasa() < r.min || c->tasa() > r.max) {
            ostringstream os;
            os << "Tasa " << c->tasa() << "% fuera del rango permitido (" << r.min << "% - " << r.max << "%) para " << c->tipo();
            throw invalid_argument(os.str());
        }
        cuentas_[s.id] = c;
        return c;
    }
    shared_ptr<Cuenta> buscar(const string& id) const {
        lock_guard<mutex> lk(mtx_);
        auto it = cuentas_.find(id);
        return it == cuentas_.end() ? nullptr : it->second;
    }
    vector<shared_ptr<Cuenta>> cuentasDe(const string& titular) const {
        lock_guard<mutex> lk(mtx_);
        vector<shared_ptr<Cuenta>> r;
        for (const auto& kv : cuentas_) if (kv.second->esTitular(titular)) r.push_back(kv.second);
        return r;
    }
    void cierreDeMes(Fecha f) {                          // operacion de la entidad: intereses de todas las cuentas
        lock_guard<mutex> lk(mtx_);
        for (auto& kv : cuentas_) kv.second->aplicarInteres(f);
    }
    size_t cantidad() const { lock_guard<mutex> lk(mtx_); return cuentas_.size(); }
};

// ================= Acceso por cliente: cada cliente usa SU propia instancia de Sesion =================
class Sesion {
    string cliente_;
    RepositorioCuentas& repo_;                           // todas las sesiones apuntan al MISMO repositorio

    shared_ptr<Cuenta> propia(const string& id) const {
        auto c = repo_.buscar(id);
        if (!c) throw invalid_argument("No existe la cuenta " + id);
        if (!c->esTitular(cliente_)) throw invalid_argument(cliente_ + " no es titular de " + id);
        return c;
    }
    string estado(Fecha corte, const function<bool(const Cuenta&)>& filtro, const string& titulo) const {
        ostringstream os;
        os << fixed << setprecision(2);
        os << "\n==== ESTADO DE CUENTA (" << titulo << ") de " << cliente_ << " al " << corte.str() << " ====\n";
        double total = 0;
        for (const auto& c : repo_.cuentasDe(cliente_)) {
            if (!filtro(*c)) continue;
            os << "\n[" << c->tipo() << "] " << c->id() << " tasa " << c->tasa() << "%";
            if (c->mancomunada()) {
                os << " MANCOMUNADA: ";
                for (size_t i = 0; i < c->titulares().size(); ++i) os << (i ? " y " : "") << c->titulares()[i];
            }
            os << "\n";
            for (const auto& m : c->movimientos())
                if (m.fecha <= corte) os << "  " << m.fecha.str() << "  " << left << setw(46) << m.desc << right << setw(11) << m.monto << "\n";
            double s = c->saldoAl(corte);
            os << "  Saldo al corte: S/ " << s << (c->esDeuda() ? "  (deuda)" : "") << "\n";
            total += s;
        }
        os << "\nSaldo neto: S/ " << total << "\n";
        return os.str();
    }
public:
    explicit Sesion(string cliente) : cliente_(move(cliente)), repo_(RepositorioCuentas::instancia()) {}
    const string& cliente() const { return cliente_; }
    const RepositorioCuentas* repositorio() const { return &repo_; }

    shared_ptr<Cuenta> abrirCuenta(Solicitud s) {        // el cliente de la sesion siempre es titular
        if (find(s.titulares.begin(), s.titulares.end(), cliente_) == s.titulares.end()) s.titulares.insert(s.titulares.begin(), cliente_);
        return repo_.abrir(s);
    }
    void depositar(const string& id, double monto, Fecha f, const string& desc = "Deposito") { propia(id)->depositar(monto, f, desc); }
    bool retirar(const string& id, double monto, Fecha f, const string& desc) { return propia(id)->retirar(monto, f, desc); }

    bool transferir(const string& origen, const string& destino, double monto, Fecha f) {
        auto o = propia(origen);
        auto d = repo_.buscar(destino);                  // el destino puede ser de otro cliente
        if (!d) throw invalid_argument("No existe la cuenta " + destino);
        auto base = [](const Cuenta& c) {                // comision nacional segun el tipo de la cuenta origen
            if (c.tipo() == "Plus" || c.tipo() == "Empresarial") return 0.5;
            return c.tipo() == "Joven" ? 1.0 : 2.0;
        };
        double com = monto * limitar(base(*o), 0.5, 4.0) / 100.0;
        if (!o->retirar(monto + com, f, "Transferencia a " + destino + " (incluye comision " + dinero(com) + ")")) return false;
        d->depositar(monto, f, "Transferencia de " + origen);
        return true;
    }
    bool pagarDeuda(const string& origen, const string& deuda, double monto, Fecha f) {    // adelantar (parcial)
        auto o = propia(origen), d = propia(deuda);
        if (!d->esDeuda()) throw invalid_argument(deuda + " no es una deuda");
        monto = min(monto, -d->saldo());
        if (monto <= 0 || !o->retirar(monto, f, "Pago a " + deuda)) return false;
        d->depositar(monto, f, "Pago recibido de " + origen);
        return true;
    }
    bool liquidarDeuda(const string& origen, const string& deuda, Fecha f) { return pagarDeuda(origen, deuda, -propia(deuda)->saldo(), f); }

    string estadoDeCuenta(Fecha corte) const { return estado(corte, [](const Cuenta&) { return true; }, "GLOBAL"); }
    string estadoDeCuenta(Fecha corte, const string& tipo) const {
        return estado(corte, [&](const Cuenta& c) { return c.tipo() == tipo; }, "TIPO: " + tipo);
    }
};

// Un tipo nuevo que la entidad crea segun el mercado
class CuentaSueldo : public Cuenta {
public:
    using Cuenta::Cuenta;
    string tipo() const override { return "Sueldo"; }
    RangoTasa rango() const override { return {0.5, 1.5}; }
};

int main() {
    Sesion ana("Ana Perez"), luis("Luis Rojas");         // dos instancias de acceso distintas...
    cout << "Sesiones distintas: " << (&ana != &luis ? "si" : "no") << "\n";
    cout << "...pero el mismo repositorio: " << (ana.repositorio() == luis.repositorio() ? "si" : "no") << "\n";
    bool mismoEnHilo = false;
    thread t([&] { mismoEnHilo = (&RepositorioCuentas::instancia() == ana.repositorio()); });
    t.join();
    cout << "Un hilo distinto ve el mismo repositorio: " << (mismoEnHilo ? "si" : "no") << "\n";

    ana.abrirCuenta({"Ahorro", "AH-001", {}, 0.8});
    ana.abrirCuenta({"Plus", "PL-001", {"Luis Rojas"}, 3.5});                 // mancomunada Ana y Luis
    ana.abrirCuenta({"Tarjeta", "TC-001", {}, 22.0, 5000});
    Solicitud sp{"Prestamo", "PR-001", {}, -1, 10000, 80, 12, Fecha(2026, 11, 2)};
    auto prest = ana.abrirCuenta(sp);
    luis.abrirCuenta({"Joven", "JV-001", {}, 1.8});

    // La entidad registra un tipo nuevo en el repositorio unico y Luis lo usa
    RepositorioCuentas::instancia().registrarTipo("Sueldo", [](const Solicitud& s) { return make_shared<CuentaSueldo>(s.id, s.titulares, s.tasa); });
    luis.abrirCuenta({"Sueldo", "SU-001", {}, 1.0});

    cout << fixed << setprecision(2);
    cout << "Cuentas en el repositorio: " << RepositorioCuentas::instancia().cantidad() << "\n";
    cout << "Tasa del prestamo con calificacion 80: " << prest->tasa() << "%\n";

    ana.depositar("AH-001", 3000, Fecha(2026, 11, 1), "Deposito inicial");
    ana.depositar("PL-001", 1500, Fecha(2026, 11, 1), "Deposito inicial");
    ana.depositar("AH-001", 10000, Fecha(2026, 11, 2), "Desembolso prestamo PR-001");
    ana.retirar("TC-001", 1200, Fecha(2026, 11, 5), "Compra supermercado");
    ana.retirar("TC-001", 800, Fecha(2026, 11, 8), "Compra electronica");
    ana.transferir("AH-001", "PL-001", 500, Fecha(2026, 11, 10));
    ana.transferir("AH-001", "JV-001", 200, Fecha(2026, 11, 12));             // a la cuenta de otro cliente
    luis.depositar("PL-001", 300, Fecha(2026, 11, 14), "Aporte de Luis");      // Luis tambien es titular de PL-001
    luis.depositar("SU-001", 2500, Fecha(2026, 11, 15), "Abono de sueldo");

    try { luis.depositar("AH-001", 50, Fecha(2026, 11, 16)); }                 // Luis NO es titular de AH-001
    catch (const invalid_argument& e) { cout << "Error esperado: " << e.what() << "\n"; }
    try { ana.abrirCuenta({"Ahorro", "AH-002", {}, 5.0}); }
    catch (const invalid_argument& e) { cout << "Error esperado: " << e.what() << "\n"; }

    RepositorioCuentas::instancia().cierreDeMes(Fecha(2026, 11, 30));

    auto* p = dynamic_cast<PrestamoPersonal*>(prest.get());
    cout << "Cuota mensual del prestamo (12 cuotas): " << p->cuota() << "\n";
    ana.pagarDeuda("AH-001", "PR-001", p->cuota(), Fecha(2026, 12, 1)); p->cuotaPagada();
    ana.pagarDeuda("AH-001", "PR-001", 2000, Fecha(2026, 12, 2));              // adelanto de capital
    cout << "Cuota despues de adelantar 2000: " << p->cuota() << "\n";
    ana.liquidarDeuda("AH-001", "TC-001", Fecha(2026, 12, 3));

    cout << ana.estadoDeCuenta(Fecha(2026, 12, 5));
    cout << ana.estadoDeCuenta(Fecha(2026, 12, 5), "Plus");
    cout << luis.estadoDeCuenta(Fecha(2026, 12, 5));
    return 0;
}