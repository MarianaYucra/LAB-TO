#include <iostream>
#include <string>
using namespace std;

class CepaAlfa {
protected:
    string proteinaSpikeAlfa;
public:
    CepaAlfa() : proteinaSpikeAlfa("Mutacion N501Y") {}
    void mostrarMutacionAlfa() {
        cout << "Propiedad Alfa: " << proteinaSpikeAlfa << endl;
    }
};

class CepaDelta {
protected:
    string proteinaSpikeDelta;
public:
    CepaDelta() : proteinaSpikeDelta("Mutacion L452R") {}
    void mostrarMutacionDelta() {
        cout << "Propiedad Delta: " << proteinaSpikeDelta << endl;
    }
};

// Cepa recombinante que hereda caracteristicas de Alfa y Delta
class CepaHibrida : public CepaAlfa, public CepaDelta {
private:
    string nombreCepa;
public:
    CepaHibrida(string nombre) : CepaAlfa(), CepaDelta(), nombreCepa(nombre) {}
    
    void mostrarDetalles() {
        cout << "Cepa Recombinante: " << nombreCepa << endl;
        mostrarMutacionAlfa();
        mostrarMutacionDelta();
    }
};

int main() {
    CepaHibrida virus("Variant-X");
    virus.mostrarDetalles();
    return 0;
}