#include <iostream>
#include "FooManager.h"

FooManager* FooManager::m_Inst = nullptr;

FooManager::FooManager() : SampleData(100) {
    std::cout << "[FooManager] instancia creada" << std::endl;
}

FooManager::~FooManager() {
    std::cout << "[FooManager] instancia destruida" << std::endl;
}

void FooManager::Destroy() {
    delete m_Inst;
    m_Inst = nullptr;
}