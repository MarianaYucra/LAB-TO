#include "task.h"
#include "ui_task.h"
#include <QDebug>

Task::Task(const QString &texto, QWidget *parent)
    : QWidget(parent), ui(new Ui::Task)
{
    ui->setupUi(this);
    ui->checkbox->setText(texto);
    connect(ui->removeButton, &QPushButton::clicked, [this]{ emit removed(this); });
}

Task::~Task()
{
    qDebug() << "objeto eliminado...";
    delete ui;
}
