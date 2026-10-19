#include <QCoreApplication>
#include <QDebug>
#include <vector>
#include <algorithm>
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int (*s)(int) = [](int valor){ return valor + 3; };
    int u = 10;
    qDebug() << s(u);                       // 13

    auto R = [](int valor){ return valor; };
    qDebug() << R(5);

    vector<int> v {5,2,3,7,1,0,14,20};
    for_each(v.begin(), v.end(), [](int valor){
        qDebug() << valor;
    });

    return 0;
}
