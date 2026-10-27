#ifndef TASK_H
#define TASK_H

#include <QWidget>
#include <QString>

namespace Ui { class Task; }

class Task : public QWidget
{
    Q_OBJECT
public:
    explicit Task(const QString &texto = "Subir - Ascender", QWidget *parent = nullptr);
    ~Task() override;
    void setTexto(const QString &texto);
    QString texto() const;

private slots:
    void editar();

signals:
    void removed(Task *task);

private:
    Ui::Task *ui;
};
#endif // TASK_H