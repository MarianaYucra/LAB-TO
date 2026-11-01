#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <iomanip>

std::mutex vectLock;
std::vector<unsigned int> primeVect;

// ---------- Versión secuencial ----------
void FindPrimesWithoutThreads(unsigned int start, unsigned int end,
                              std::vector<unsigned int>& vect) {
    if (start % 2 == 0) ++start;                  // solo impares
    for (unsigned int x = start; x <= end; x += 2) {
        for (unsigned int y = 2; y < x; ++y) {
            if ((x % y) == 0) {
                break;
            } else if ((y + 1) == x) {
                vect.push_back(x);
            }
        }
    }
}

// ---------- Versión paralela ----------
void FindPrimes(unsigned int start, unsigned int end) {
    if (start % 2 == 0) ++start;
    for (unsigned int x = start; x <= end; x += 2) {
        for (unsigned int y = 2; y < x; ++y) {
            if ((x % y) == 0) {
                break;
            } else if ((y + 1) == x) {
                std::lock_guard<std::mutex> lk(vectLock);   // sección crítica
                primeVect.push_back(x);
            }
        }
    }
}

void FindPrimesWithThreads(unsigned int start, unsigned int end, unsigned int numThreads) {
    std::vector<std::thread> threadVect;
    unsigned int total = end - start + 1;
    unsigned int threadSpread = total / numThreads;
    unsigned int newEnd = start + threadSpread - 1;

    for (unsigned int i = 0; i < numThreads; ++i) {
        if (i == numThreads - 1) newEnd = end;    // el último hilo toma el resto
        threadVect.emplace_back(FindPrimes, start, newEnd);
        start = newEnd + 1;
        newEnd = start + threadSpread - 1;
    }
    for (auto& t : threadVect) t.join();
}

double segundos(std::chrono::steady_clock::time_point a, std::chrono::steady_clock::time_point b) {
    return std::chrono::duration<double>(b - a).count();
}

int main() {
    const unsigned int LIMITE = 100000;
    std::cout << std::fixed << std::setprecision(3);

    std::vector<unsigned int> secuencial;
    auto t0 = std::chrono::steady_clock::now();
    FindPrimesWithoutThreads(1, LIMITE, secuencial);
    auto t1 = std::chrono::steady_clock::now();
    std::cout << "SIN HILOS      : " << segundos(t0, t1) << " s, primos = " << secuencial.size() << "\n";
    std::sort(secuencial.begin(), secuencial.end());

    for (unsigned int nh : {1u, 2u, 4u, 8u}) {
        primeVect.clear();
        t0 = std::chrono::steady_clock::now();
        FindPrimesWithThreads(1, LIMITE, nh);
        t1 = std::chrono::steady_clock::now();
        std::sort(primeVect.begin(), primeVect.end());
        bool igual = (primeVect == secuencial);
        std::cout << "CON " << nh << " HILO(S)   : " << segundos(t0, t1) << " s, primos = "
                  << primeVect.size() << (igual ? "  [OK, igual al secuencial]" : "  [ERROR]") << "\n";
    }
    return 0;
}