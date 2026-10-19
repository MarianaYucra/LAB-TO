#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    datos["Perú"]     = qMakePair(QString("Español"),   QString("Lima"));
    datos["Brasil"]   = qMakePair(QString("Portugués"), QString("Brasilia"));
    datos["Francia"]  = qMakePair(QString("Francés"),   QString("París"));
    datos["Japón"]    = qMakePair(QString("Japonés"),   QString("Tokio"));
    datos["Alemania"] = qMakePair(QString("Alemán"),    QString("Berlín"));
    datos["Italia"]   = qMakePair(QString("Italiano"),  QString("Roma"));

    for (auto it = datos.constBegin(); it != datos.constEnd(); ++it)
        ui->listaPaises->addItem(it.key());

    connect(ui->listaPaises, &QListWidget::itemClicked,
            this, &MainWindow::mostrarInfo);
}

void MainWindow::mostrarInfo(QListWidgetItem *item)
{
    QString pais = item->text();
    ui->infoLabel->setText(QString("%1 → Idioma: %2 | Capital: %3")
                               .arg(pais, datos[pais].first, datos[pais].second));
}

MainWindow::~MainWindow()
{
    delete ui;
}