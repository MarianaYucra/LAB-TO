#include <QCoreApplication>
#include "Comunicacion.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QObject *person = new QObject;
    Comunicacion *pepe = new Comunicacion(person);
    Comunicacion *jose = new Comunicacion(person);
    pepe->setNombre("pepe");
    jose->setNombre("jose");

    QObject::connect(pepe, &Comunicacion::enviarmensaje, jose, &Comunicacion::escuchar);
    QObject::connect(jose, &Comunicacion::enviarmensaje, pepe, &Comunicacion::escuchar);

    pepe->conversar("buenos dias");
    jose->conversar("como vas?");

    delete person;
    return 0;
}