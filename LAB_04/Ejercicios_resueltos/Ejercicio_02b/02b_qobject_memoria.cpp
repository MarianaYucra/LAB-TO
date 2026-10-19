#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QString>

class adulto : public QObject
{
private:
    QString nombre;
public:
    adulto(QObject *base = 0) : QObject(base) {}
    void setNombre(const QString &nomb) { nombre = nomb; }
    QString getNombre() const { return nombre; }
    ~adulto() { qDebug() << "..objeto destruido..."; }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    adulto *papa  = new adulto;
    adulto *hijo1 = new adulto(papa);
    adulto *hijo2 = new adulto(papa);
    hijo1->setNombre("manuel");
    hijo2->setNombre("Jose");

    delete papa;
    return 0;
}