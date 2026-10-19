#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QPair>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void mostrarInfo(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QMap<QString, QPair<QString, QString>> datos;   // país -> (idioma, capital)
};
#endif // MAINWINDOW_H