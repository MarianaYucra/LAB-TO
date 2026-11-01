#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <thread>
#include <functional>   // para std::ref
#include <cmath>
#include <stdexcept>
#include <algorithm>

struct Fila {
    std::string origen, destino;
    double x1, y1, x2, y2;
    double euclidea = 0.0, manhattan = 0.0;
};

std::vector<Fila> leerCSV(const std::string& ruta) {
    std::ifstream f(ruta);
    if (!f) throw std::runtime_error("No se pudo abrir " + ruta);
    std::vector<Fila> datos;
    std::string linea;
    std::getline(f, linea);                       // salta la cabecera
    while (std::getline(f, linea)) {
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        if (linea.empty()) continue;
        std::stringstream ss(linea);
        std::string c[6];
        for (int i = 0; i < 6; ++i) std::getline(ss, c[i], ',');
        Fila r;
        r.origen = c[0]; r.destino = c[1];
        r.x1 = std::stod(c[2]); r.y1 = std::stod(c[3]);
        r.x2 = std::stod(c[4]); r.y2 = std::stod(c[5]);
        datos.push_back(r);
    }
    return datos;
}

// Cada hilo escribe SOLO en sus posiciones [ini, fin)
void calcular(std::vector<Fila>& d, std::size_t ini, std::size_t fin) {
    for (std::size_t i = ini; i < fin; ++i) {
        double dx = d[i].x2 - d[i].x1, dy = d[i].y2 - d[i].y1;
        d[i].euclidea  = std::sqrt(dx * dx + dy * dy);
        d[i].manhattan = std::fabs(dx) + std::fabs(dy);
    }
}

int main(int argc, char* argv[]) {
    std::string ruta = (argc > 1) ? argv[1] : "distancias_manhattan_euclides.csv";
    try {
        std::vector<Fila> datos = leerCSV(ruta);
        std::size_t n = datos.size();
        std::size_t nh = std::max<std::size_t>(1, std::min<std::size_t>(4, n));

        std::vector<std::thread> hilos;
        for (std::size_t i = 0; i < nh; ++i)
            hilos.emplace_back(calcular, std::ref(datos), i * n / nh, (i + 1) * n / nh);
        for (auto& h : hilos) h.join();

        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Origen -> Destino | Euclides | Manhattan\n";
        for (std::size_t i = 0; i < std::min<std::size_t>(5, n); ++i)
            std::cout << datos[i].origen << " -> " << datos[i].destino << " | "
                      << datos[i].euclidea << " | " << datos[i].manhattan << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}