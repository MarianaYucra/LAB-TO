#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <ctime>
#include <random>
#include <mutex>

std::mutex mtxCout;

// GetRandom no viene en la guía: devuelve un entero entre 1 y max
int GetRandom(int max) {
    static std::mutex m;
    static std::mt19937 gen(std::random_device{}());
    std::lock_guard<std::mutex> lk(m);
    return std::uniform_int_distribution<int>(1, max)(gen);
}

// localtime no es segura entre hilos; se usa la versión segura de cada sistema
std::tm horaLocal(std::time_t t) {
    std::tm r{};
#ifdef _WIN32
    localtime_s(&r, &t);
#else
    localtime_r(&t, &r);
#endif
    return r;
}

std::string formato(std::time_t t) {
    std::tm r = horaLocal(t);
    std::ostringstream os;
    os << std::put_time(&r, "%a %b %d %H:%M:%S %Y");
    return os.str();
}

void ExecuteThread(int id) {
    std::time_t sleepTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm t = horaLocal(sleepTime);

    std::ostringstream ini;                       // se arma todo el texto y se imprime de una vez
    ini << "Thread: " << id << " sleep Time: " << formato(sleepTime) << "\n"
        << "Month: " << t.tm_mon + 1 << "\n"
        << "Day: " << t.tm_mday << "\n"
        << "Year: " << t.tm_year << "\n"
        << "Hours: " << t.tm_hour << "\n"
        << "Minutes: " << t.tm_min << "\n"
        << "Seconds: " << t.tm_sec << "\n";
    { std::lock_guard<std::mutex> lk(mtxCout); std::cout << ini.str() << std::endl; }

    std::this_thread::sleep_for(std::chrono::seconds(GetRandom(3)));

    std::time_t awake = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::lock_guard<std::mutex> lk(mtxCout);
    std::cout << "Thread " << id << " Awake Time : " << formato(awake) << "\n" << std::endl;
}

int main() {
    std::cout << "=== VERSION 1: join inmediato (secuencial) ===\n" << std::endl;
    { std::thread th1(ExecuteThread, 1); th1.join();
      std::thread th2(ExecuteThread, 2); th2.join(); }

    std::cout << "=== VERSION 2: crear ambos y luego join (paralelo) ===\n" << std::endl;
    { std::thread th1(ExecuteThread, 1);
      std::thread th2(ExecuteThread, 2);
      th1.join(); th2.join(); }
    return 0;
}