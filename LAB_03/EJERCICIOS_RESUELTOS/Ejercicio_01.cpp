#include <iostream>
#include <string>
using namespace std;

class infante {
private:
    string nombre;
public:
    infante(const string &nom) {
        nombre = nom;
    }
    void gatear() {
        cout << nombre << " gateando...\n";
    }
};

class joven {
private:
    string nombre;
public:
    joven(const string &nom) {
        nombre = nom;
    }
    void correr() {
        cout << nombre << " corriendo...\n";
    }
};

class adulto : public infante, public joven {
private:
    string nombre;
public:
    adulto(const string &nom) : infante(nom), joven(nom), nombre(nom) {}
    void caminar() {
        cout << nombre << " caminando...\n";
    }
};

int main() {
    adulto a("Carlos");
    a.gatear();
    a.correr();
    a.caminar();
    return 0;
}