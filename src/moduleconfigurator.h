#ifndef MODULECONFIGURATOR_H
#define MODULECONFIGURATOR_H

#include "../helper/moduleconfiguration.h"

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class ModuleConfigurator;
}

class ModuleConfigurator : public QDialog {
  Q_OBJECT

public:
  explicit ModuleConfigurator(QWidget *parent = nullptr);
  ~ModuleConfigurator();
  bool startConfiguration(ModuleConfiguration &config);

private:
  Ui::ModuleConfigurator *ui;
  QStandardItemModel mParameterModel;

  void populateUi(ModuleConfiguration &config);
};

#endif // MODULECONFIGURATOR_H
