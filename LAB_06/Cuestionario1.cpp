#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool listo = false;

void esperar_trabajo() {
    std::unique_lock<std::mutex> lock(mtx);
    
    // cv.wait libera el mutex y duerme al hilo hasta recibir notificación
    // El segundo argumento (predicado) evita despertadores espurios (spurious wakeups)
    cv.wait(lock, [] { return listo; });
    
    std::cout << "Hilo trabajador: Recurso procesado exitosamente.\n";
}

void preparar_trabajo() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        listo = true;
        std::cout << "Hilo principal: Preparando datos...\n";
    } // El mutex se libera aquí al salir del scope
    
    cv.notify_one(); // Notifica al hilo en espera
}

int main() {
    std::thread t1(esperar_trabajo);
    std::thread t2(preparar_trabajo);

    t1.join();
    t2.join();
    return 0;
}