#pragma once
#include <iostream>
#include <string>
using namespace std;

class Toy {
protected:
    string name;
    float price;
public:
    Toy(string n, float p) : name(n), price(p) {}      // la guia nunca inicializaba name ni price
    virtual ~Toy() {}                                  // destructor virtual: obligatorio para borrar por Toy*
    virtual void prepareParts() = 0;
    virtual void combineParts() = 0;
    virtual void assembleParts() = 0;
    virtual void applyLabel() = 0;
    void showProduct() const { cout << "Name: " << name << endl << "Price: " << price << endl; }
};

class Car : public Toy {
public:
    Car() : Toy("Car", 25.50f) {}
    void prepareParts() override  { cout << "Preparando partes de Car" << endl; }
    void combineParts() override  { cout << "Combinando partes de Car" << endl; }
    void assembleParts() override { cout << "ensamblando partes de Car" << endl; }
    void applyLabel() override    { cout << "aplicando etiquetado de Car" << endl; }
};

class Bike : public Toy {
public:
    Bike() : Toy("Bike", 40.00f) {}
    void prepareParts() override  { cout << "Preparando partes de Bike" << endl; }
    void combineParts() override  { cout << "Combinando partes de Bike" << endl; }
    void assembleParts() override { cout << "ensamblando partes de Bike" << endl; }
    void applyLabel() override    { cout << "aplicando etiquetado de Bike" << endl; }
};

class Plane : public Toy {
public:
    Plane() : Toy("Plane", 60.75f) {}
    void prepareParts() override  { cout << "Preparando partes de Plane" << endl; }
    void combineParts() override  { cout << "Combinando partes de Plane" << endl; }
    void assembleParts() override { cout << "ensamblando partes de Plane" << endl; }
    void applyLabel() override    { cout << "aplicando etiquetado de Plane" << endl; }
};