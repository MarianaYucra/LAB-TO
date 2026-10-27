#include "task.h"
#include "ui_task.h"
#include <QInputDialog>
#include <QDebug>

Task::Task(const QString &texto, QWidget *parent)
    : QWidget(parent), ui(new Ui::Task)
{
    ui->setupUi(this);
    ui->checkbox->setChecked(false);
    ui->editButton->setText("Editar");
    setTexto(texto);
    connect(ui->editButton, &QPushButton::clicked, this, &Task::editar);
    connect(ui->removeButton, &QPushButton::clicked, [this]{ emit removed(this); });
}

Task::~Task() { qDebug() << "objeto eliminado..."; delete ui; }

void Task::setTexto(const QString &texto) { ui->checkbox->setText(texto); }
QString Task::texto() const { return ui->checkbox->text(); }

void Task::editar()
{
    bool ok = false;
    QString nuevo = QInputDialog::getText(this, "Editar sinónimos",
                                          "Sinónimos:", QLineEdit::Normal, texto(), &ok);
    if (ok && !nuevo.trimmed().isEmpty())
        setTexto(nuevo.trimmed());
}