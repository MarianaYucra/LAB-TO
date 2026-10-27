#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "task.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), mTasks()
{
    ui->setupUi(this);
    connect(ui->addTaskButton, &QPushButton::clicked, this, &MainWindow::addTask);
}

void MainWindow::addTask()
{
    qDebug() << "Slot corresponde al clic en el boton ...";
    Task *task = new Task("Tarea sin titulo");
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