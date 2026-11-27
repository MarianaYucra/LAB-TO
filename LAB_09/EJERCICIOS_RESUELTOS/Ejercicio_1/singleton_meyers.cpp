#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
using namespace std;

class FooManager {
public:
    static FooManager& instancia() {             // C++11: la inicializacion de una variable estatica local es segura entre hilos
        static FooManager inst;
        return inst;
    }
    FooManager(const FooManager&) = delete;
    FooManager& operator=(const FooManager&) = delete;
    int GetSampleData() const { return SampleData; }
private:
    FooManager() : SampleData(100) { cout << "[FooManager] instancia creada\n"; }
    int SampleData;
};

int main() {
    atomic<int> distintas{0};
    const FooManager* referencia = &FooManager::instancia();
    vector<thread> hilos;
    for (int i = 0; i < 4; ++i)
        hilos.emplace_back([&] { if (&FooManager::instancia() != referencia) ++distintas; });
    for (auto& h : hilos) h.join();
    cout << "Datos: " << FooManager::instancia().GetSampleData() << "\n";
    cout << "Hilos que vieron otra instancia: " << distintas << "\n";
    return 0;
}