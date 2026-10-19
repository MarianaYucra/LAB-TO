#ifndef COMUNICACION_H
#define COMUNICACION_H
#include <QObject>
#include <QString>

class Comunicacion : public QObject
{
    Q_OBJECT
public:
    explicit Comunicacion(QObject *parent = nullptr);
    void setNombre(const QString &nomb) { strnombre = nomb; }
    void conversar(const QString &conversa);

signals:
    void enviarmensaje(QString);

public slots:
    void escuchar(const QString &str);

private:
    QString strnombre;
};
#endif