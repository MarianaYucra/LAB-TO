#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "task.h"
#include <QDebug>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), mTasks()
{
    ui->setupUi(this);
    connect(ui->addTaskButton, &QPushButton::clicked, this, &MainWindow::addTask);
}

void MainWindow::addTask()
{
    static const QStringList sinonimos = {
        "Subir - Ascender", "Bajar - Descender",
        "Rápido - Veloz", "Grande - Enorme", "Bonito - Hermoso"
    };
    QString texto = sinonimos.at(mTasks.size() % sinonimos.size());

    Task *task = new Task(texto);
    connect(task, &Task::removed, this, &MainWindow::removeTask);
    mTasks.append(task);
    ui->tasksLayout->addWidget(task);
}
void MainWindow::removeTask(Task *task)
{
    mTasks.removeOne(task);
    ui->tasksLayout->removeWidget(task);
    task->deleteLater();
}

MainWindow::~MainWindow()
{
    delete ui;
}