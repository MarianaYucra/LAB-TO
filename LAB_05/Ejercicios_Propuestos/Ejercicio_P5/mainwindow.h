#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector> // Necesario para usar QVector

// Forward declaration de la clase Task
class Task;

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

public slots:
    void addTask();
    void removeTask(Task *task);

private:
    Ui::MainWindow *ui;
    QVector<Task*> mTasks; // Vector para almacenar los punteros a Task
};

#endif // MAINWINDOW_H
