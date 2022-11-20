#ifndef MODULECONFIGURATOR_H
#define MODULECONFIGURATOR_H

#include "../helper/moduleconfiguration.h"

#include <QDialog>

namespace Ui {
class ModuleConfigurator;
}

class ModuleConfigurator : public QDialog {
  Q_OBJECT

public:
  explicit ModuleConfigurator(QWidget *parent = nullptr);
  ~ModuleConfigurator();

private:
  Ui::ModuleConfigurator *ui;
};

#endif // MODULECONFIGURATOR_H
