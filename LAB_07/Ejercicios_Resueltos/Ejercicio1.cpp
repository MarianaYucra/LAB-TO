#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ---------- Jerarquía de componentes ----------
class item {
public:
    virtual ~item() {}                                   // destructor virtual: obligatorio al borrar por puntero base
    virtual string GetName() const { return ""; }
    virtual float GetPrice() const { return 0.0f; }
    virtual void GetConfiguration() const {}
};

class Processor : public item {};
class IntelI5 : public Processor {
public:
    string GetName() const override { return "Intel i5"; }
    float GetPrice() const override { return 3000.00f; }
    void GetConfiguration() const override {
        cout << "1.50 Ghz, up to 2.0 Ghz with turbo, 2 cores, 4 threads, 8 MB cache, RAM 4 GB - 8 GB" << endl;
    }
};
class IntelI7 : public Processor {
public:
    string GetName() const override { return "Intel i7"; }
    float GetPrice() const override { return 4700.00f; }
    void GetConfiguration() const override {
        cout << "1.6 Ghz, up to 1.99 Ghz, 9na Gen, 4 cores, 8 threads, 16 MB cache, RAM 16 GB" << endl;
    }
};

class Drive : public item {};
class HDD : public Drive {
public:
    string GetName() const override { return "Hard disk drive"; }
    float GetPrice() const override { return 150.00f; }
    void GetConfiguration() const override {
        cout << "capacity: 1 TB, data transfer 1050 mbits/s" << endl;
    }
};
class SDD : public Drive {
public:
    string GetName() const override { return "Solid state drive"; }
    float GetPrice() const override { return 270.00f; }
    void GetConfiguration() const override {
        cout << "capacity: 250 GB, data transfer 1800 mbits/s" << endl;
    }
};

class DisplayType : public item {};
class Normal : public DisplayType {
public:
    string GetName() const override { return "Non-touch screen"; }
    float GetPrice() const override { return 195.00f; }
    void GetConfiguration() const override {
        cout << "15.6 inch FHD(1920 x 1080), plane, ....etc" << endl;
    }
};

// ---------- Producto ----------
class laptop {
public:
    laptop() {}
    laptop(const laptop&) = delete;
    laptop& operator=(const laptop&) = delete;
    ~laptop() { for (item* p : mLaptopParts) delete p; }   // la laptop es dueña de sus partes

    void AddParts(item* parte) { mLaptopParts.push_back(parte); }

    float GetCost() const {
        float cost = 0.0f;
        for (const item* p : mLaptopParts) cost += p->GetPrice();
        return cost;
    }
    void GetConfiguration() const {
        for (size_t i = 0; i < mLaptopParts.size(); i++) {
            cout << mLaptopParts[i]->GetName() << " : ";
            mLaptopParts[i]->GetConfiguration();
        }
    }
private:
    vector<item*> mLaptopParts;
};

// ---------- "Builder" de la guía: dos configuraciones fijas ----------
class laptopBuilder {
public:
    laptop* opcion1() {
        laptop* lap = new laptop();
        lap->AddParts(new IntelI5());
        lap->AddParts(new Normal());
        lap->AddParts(new HDD());
        return lap;
    }
    laptop* opcion2() {
        laptop* lap = new laptop();
        lap->AddParts(new IntelI7());
        lap->AddParts(new Normal());
        lap->AddParts(new SDD());
        return lap;
    }
};

int main() {
    laptopBuilder cotizar;

    cout << "Laptop Lenovo XY \n";
    laptop* lenovo = cotizar.opcion1();
    lenovo->GetConfiguration();
    cout << "precio: " << lenovo->GetCost() << endl;
    delete lenovo;

    cout << "\nLaptop Lenovo XZ \n";
    laptop* otra = cotizar.opcion2();
    otra->GetConfiguration();
    cout << "precio: " << otra->GetCost() << endl;
    delete otra;
    return 0;
}