#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <mutex>

std::mutex salida;

void* procesoHilo(void* arg) {
    const char* nombre = static_cast<const char*>(arg);
    for (int i = 1; i <= 5; ++i) {
        { std::lock_guard<std::mutex> lock(salida);
          std::cout << nombre << " - iteracion " << i << '\n'; }
        sleep(1);
    }
    return nullptr;
}

int main() {
    pthread_t proceso1, proceso2;
    char nombre1[] = "Proceso 1";
    char nombre2[] = "Proceso 2";

    if (pthread_create(&proceso1, nullptr, procesoHilo, nombre1) != 0 ||
        pthread_create(&proceso2, nullptr, procesoHilo, nombre2) != 0) {
        std::cerr << "Error al crear los hilos\n";
        return 1;
    }
    pthread_join(proceso1, nullptr);
    pthread_join(proceso2, nullptr);
    std::cout << "Ambos hilos finalizaron.\n";
    return 0;
}