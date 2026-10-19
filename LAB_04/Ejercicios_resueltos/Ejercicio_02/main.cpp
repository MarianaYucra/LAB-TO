#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <string>
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int x = 47;
    double y = 3.2478;
    QString t = "Ok";                       // 'Ok' como char daría warning

    QString qtcadena = QString("%1 %2 %3").arg(t).arg(y).arg(x);
    string cadena = qtcadena.toStdString();
    qDebug() << cadena.c_str();
    qDebug() << qtcadena;

    qtcadena = "arequipa ciudad blanca 2021";
    qDebug() << qtcadena.contains("ciudad");   // true
    qDebug() << qtcadena.indexOf("2021");      // 23

    return 0;
}