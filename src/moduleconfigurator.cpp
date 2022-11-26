#include <QStandardItem>

#include "moduleconfigurator.h"
#include "ui_moduleconfigurator.h"

#include <QFile>

ModuleConfigurator::ModuleConfigurator(QWidget *parent)
    : QDialog(parent), ui(new Ui::ModuleConfigurator) {
  ui->setupUi(this);
  mParameterModel.setHorizontalHeaderItem(0, new QStandardItem("Parameter"));
  mParameterModel.setHorizontalHeaderItem(1, new QStandardItem("Value"));
  mParameterModel.setHorizontalHeaderItem(2, new QStandardItem("Unit"));
  ui->tvParameters->setModel(&mParameterModel);
}

ModuleConfigurator::~ModuleConfigurator() { delete ui; }

bool ModuleConfigurator::startConfiguration(ModuleConfiguration &config) {

  populateUi(config);
  auto dialogResult = this->exec();
  if (dialogResult == QDialog::Accepted) {

  } else {
    return false;
  }

  return true;
}

void ModuleConfigurator::populateUi(ModuleConfiguration &config) {
  setWindowTitle(config.name());

  for (int i = 1; i < mParameterModel.rowCount(); i++) {
    // never remove header, just possible previously added rows
    mParameterModel.removeRow(i);
  }

  foreach (const auto &param, config.parameters()) {
    QList<QStandardItem *> row;
    row.append(new QStandardItem(param));
    auto defaultVal = new QStandardItem();
    defaultVal->setData(config.defaultValue(param), Qt::EditRole);
    row << defaultVal;
    auto unit = new QStandardItem("test");
    row << unit;
    mParameterModel.appendRow(row);
  }
  ui->tvParameters->adjustSize();
}
