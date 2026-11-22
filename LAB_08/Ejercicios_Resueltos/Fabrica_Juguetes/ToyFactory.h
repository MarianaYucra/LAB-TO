#pragma once
#include <memory>
#include "Object.h"

class ToyFactory {
public:
    static unique_ptr<Toy> createToy(int type) {
        unique_ptr<Toy> toy;
        switch (type) {
        case 1: toy = make_unique<Car>();   break;
        case 2: toy = make_unique<Bike>();  break;
        case 3: toy = make_unique<Plane>(); break;
        default:
            cout << "invalido TOY type, ingresa otra opcion" << endl;
            return nullptr;
        }
        toy->prepareParts();      // linea de produccion: el orden de los pasos es fijo
        toy->combineParts();
        toy->assembleParts();
        toy->applyLabel();
        return toy;
    }
};