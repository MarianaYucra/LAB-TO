#include <iostream>
using namespace std;

class Mamifero {
public:
    Mamifero() : edad(0) { 
        cout << "mamifero constructor...\n"; 
    }
    virtual ~Mamifero() { 
        cout << "mamifero destructor...\n"; 
    }
    void Move() const { 
        cout << "mamifero move one step\n"; 
    }
    virtual void Speak() const { 
        cout << "mamifero speak!\n"; 
    }
protected:
    int edad;
};

class Dog : public Mamifero {
public:
    Dog() { 
        cout << "Dog Constructor...\n"; 
    }
    ~Dog() { 
        cout << "Dog destructor...\n"; 
    }
    void WagTail() { 
        cout << "Wagging Tail...\n"; 
    }
    void Speak() const override { 
        cout << "Woof!\n"; 
    }
    void Move() const { 
        cout << "Dog moves 5 steps...\n"; 
    }
};

int main() {
    Mamifero* pDog = new Dog();
    pDog->Move();
    pDog->Speak();
    delete pDog;
    return 0;
}