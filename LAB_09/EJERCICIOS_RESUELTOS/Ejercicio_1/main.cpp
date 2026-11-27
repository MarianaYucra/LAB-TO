#include <iostream>
#include "FooManager.h"
using namespace std;

int main() {
    cout << "Patron singleton" << endl;

    FooManager::Create();

    FooManager* pMyFoo1 = FooManager::GetInst();
    if (pMyFoo1 != nullptr) {
        cout << "primera instancia Inst sample data: " << pMyFoo1->GetSampleData() << endl;
        cout << pMyFoo1 << endl;
    }

    FooManager::Create();                        // llamada repetida: NO crea otro objeto
    FooManager* pMyFoo2 = FooManager::GetInst();
    if (pMyFoo2 != nullptr) {
        cout << "segunda instancia? Inst sample data: " << pMyFoo2->GetSampleData() << endl;
        cout << pMyFoo2 << endl;
    }

    cout << "Son el mismo objeto? " << (pMyFoo1 == pMyFoo2 ? "si" : "no") << endl;

    FooManager::Destroy();
    cout << "Despues de Destroy, GetInst() es nulo? " << (FooManager::GetInst() == nullptr ? "si" : "no") << endl;
    return 0;
}