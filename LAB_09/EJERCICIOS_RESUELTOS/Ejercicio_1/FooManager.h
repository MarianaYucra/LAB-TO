#pragma once

class FooManager {
public:
    int GetSampleData() const { return SampleData; }
    static void Create() {                       // crea la unica instancia la primera vez
        if (m_Inst == nullptr) {
            m_Inst = new FooManager();
        }
    }
    static FooManager* GetInst() { return m_Inst; }
    static void Destroy();                       // la guia nunca liberaba la instancia

    FooManager(const FooManager&) = delete;      // sin esto, "new FooManager(*GetInst())" crearia una segunda instancia
    FooManager& operator=(const FooManager&) = delete;

private:
    FooManager();                                // constructor y destructor privados
    ~FooManager();

    int SampleData;
    static FooManager* m_Inst;
};