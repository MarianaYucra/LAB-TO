#include <QCoreApplication>
#include <QDebug>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    mt19937 gen(random_device{}());
    uniform_int_distribution<int> dist(0, 5000);

    vector<int> v(100);
    generate(v.begin(), v.end(), [&](){ return dist(gen); });

    auto esPrimo = [](int n){
        if (n < 2) return false;
        for (int i = 2; i * i <= n; ++i)
            if (n % i == 0) return false;
        return true;
    };

    int cantidad = count_if(v.begin(), v.end(), [&](int n){
        return n < 1000 && esPrimo(n);
    });

    for (int n : v) qDebug() << n;
    qDebug() << "Primos menores de 1000:" << cantidad;

    return 0;
}