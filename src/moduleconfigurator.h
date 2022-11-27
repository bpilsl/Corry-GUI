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

private slots:
  void on_buttonBox_accepted();

  void on_buttonBox_rejected();

private:
  Ui::ModuleConfigurator *ui;
  QStandardItemModel mParameterModel;
  ModuleConfiguration *mCurrentModule;
  void populateUi(ModuleConfiguration &config);
};

#endif // MODULECONFIGURATOR_H
