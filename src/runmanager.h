#ifndef RUNMANAGER_H
#define RUNMANAGER_H

#include <QWidget>

namespace Ui {
class RunManager;
}

class RunManager : public QWidget
{
    Q_OBJECT

public:
    explicit RunManager(QWidget *parent = nullptr);
    ~RunManager();

private:
    Ui::RunManager *ui;
};

#endif // RUNMANAGER_H
