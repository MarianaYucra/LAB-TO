#include <iostream>
#include <memory>
#include <string>
using namespace std;

// ================= Productos abstractos =================
class CheckBox {
public:
    virtual ~CheckBox() = default;
    virtual void draw() const = 0;
    virtual string descripcion() const = 0;
};

class Button {
public:
    virtual ~Button() = default;
    virtual void draw() const = 0;
    // Colaboracion entre productos de la misma familia (como Funcion2_B del ejercicio b)
    virtual string interactuarCon(const CheckBox& colaborador) const = 0;
};

// ================= Productos concretos: Windows =================
class WinCheckBox : public CheckBox {
public:
    void draw() const override { cout << "Dibujando CheckBox Windows\n"; }
    string descripcion() const override { return "CheckBox Windows"; }
};
class WinButton : public Button {
public:
    void draw() const override { cout << "Dibujando Button Windows\n"; }
    string interactuarCon(const CheckBox& c) const override { return "Button Windows interactuando con " + c.descripcion(); }
};

// ================= Productos concretos: Mac =================
class MacCheckBox : public CheckBox {
public:
    void draw() const override { cout << "Dibujando CheckBox Mac\n"; }
    string descripcion() const override { return "CheckBox Mac"; }
};
class MacButton : public Button {
public:
    void draw() const override { cout << "Dibujando Button Mac\n"; }
    string interactuarCon(const CheckBox& c) const override { return "Button Mac interactuando con " + c.descripcion(); }
};

// ================= Productos concretos: Linux =================
class LinuxCheckBox : public CheckBox {
public:
    void draw() const override { cout << "Dibujando CheckBox Linux\n"; }
    string descripcion() const override { return "CheckBox Linux"; }
};
class LinuxButton : public Button {
public:
    void draw() const override { cout << "Dibujando Button Linux\n"; }
    string interactuarCon(const CheckBox& c) const override { return "Button Linux interactuando con " + c.descripcion(); }
};

// ================= Fabrica abstracta y fabricas concretas =================
class GUIFactory {
public:
    virtual ~GUIFactory() = default;
    virtual string sistema() const = 0;
    virtual unique_ptr<Button> crearButton() const = 0;
    virtual unique_ptr<CheckBox> crearCheckBox() const = 0;
};
class WinFactory : public GUIFactory {
public:
    string sistema() const override { return "Windows"; }
    unique_ptr<Button> crearButton() const override { return make_unique<WinButton>(); }
    unique_ptr<CheckBox> crearCheckBox() const override { return make_unique<WinCheckBox>(); }
};
class MacFactory : public GUIFactory {
public:
    string sistema() const override { return "Mac"; }
    unique_ptr<Button> crearButton() const override { return make_unique<MacButton>(); }
    unique_ptr<CheckBox> crearCheckBox() const override { return make_unique<MacCheckBox>(); }
};
class LinuxFactory : public GUIFactory {
public:
    string sistema() const override { return "Linux"; }
    unique_ptr<Button> crearButton() const override { return make_unique<LinuxButton>(); }
    unique_ptr<CheckBox> crearCheckBox() const override { return make_unique<LinuxCheckBox>(); }
};

// ================= Cliente: Application solo conoce las interfaces abstractas =================
class Application {
    unique_ptr<Button> button_;
    unique_ptr<CheckBox> checkBox_;
public:
    explicit Application(const GUIFactory& f) : button_(f.crearButton()), checkBox_(f.crearCheckBox()) {}
    void draw() const {
        button_->draw();
        checkBox_->draw();
        cout << button_->interactuarCon(*checkBox_) << "\n";
    }
};

// Elige la familia segun el sistema operativo (1 Windows, 2 Mac, 3 Linux)
unique_ptr<GUIFactory> crearFabrica(int os) {
    switch (os) {
    case 1: return make_unique<WinFactory>();
    case 2: return make_unique<MacFactory>();
    case 3: return make_unique<LinuxFactory>();
    default: return nullptr;
    }
}

int main() {
    for (int os : {1, 2, 3, 4}) {
        auto fabrica = crearFabrica(os);
        if (!fabrica) {
            cout << "Sistema no soportado: " << os << "\n";
            continue;
        }
        cout << "Cliente: " << fabrica->sistema() << "\n";
        Application app(*fabrica);
        app.draw();
        cout << "\n";
    }
    return 0;
}