#include <iostream>
#include <string>
using namespace std;

class Persona {
protected:
    string nombre;
    int edad;
public:
    Persona(string nom, int ed) : nombre(nom), edad(ed) {}
    virtual void realizarTrabajo() {
        cout << nombre << " realiza sus labores cotidianas.\n";
    }
    virtual ~Persona() {}
};

class Arquitecto : public Persona {
public:
    Arquitecto(string nom, int ed) : Persona(nom, ed) {}
    void realizarTrabajo() override {
        cout << nombre << " esta diseñando planos de estructuras.\n";
    }
};

class Doctor : public Persona {
public:
    Doctor(string nom, int ed) : Persona(nom, ed) {}
    void realizarTrabajo() override {
        cout << nombre << " esta diagnosticando y curando pacientes.\n";
    }
};

class Enfermera : public Persona {
public:
    Enfermera(string nom, int ed) : Persona(nom, ed) {}
    void realizarTrabajo() override {
        cout << nombre << " esta asistiendo a pacientes y aplicando cuidados.\n";
    }
};

class Bombero : public Persona {
public:
    Bombero(string nom, int ed) : Persona(nom, ed) {}
    void realizarTrabajo() override {
        cout << nombre << " esta atendiendo emergencias y apagando incendios.\n";
    }
};

int main() {
    Persona* equipo[4];
    equipo[0] = new Arquitecto("Ana", 35);
    equipo[1] = new Doctor("Carlos", 45);
    equipo[2] = new Enfermera("Elena", 29);
    equipo[3] = new Bombero("Luis", 31);

    for (int i = 0; i < 4; i++) {
        equipo[i]->realizarTrabajo();
    }

    for (int i = 0; i < 4; i++) {
        delete equipo[i];
    }

    return 0;
}