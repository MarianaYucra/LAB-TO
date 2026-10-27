#ifndef TASK_H
#define TASK_H

#include <QWidget>
#include <QString>

namespace Ui { class Task; }

class Task : public QWidget
{
    Q_OBJECT
public:
    explicit Task(const QString &texto = "Tarea sin titulo", QWidget *parent = nullptr);
    ~Task() override;

signals:
    void removed(Task *task);

private:
    Ui::Task *ui;
};
#endif // TASK_H