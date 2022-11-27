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
  mCurrentModule = &config;
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
  qDebug() << "nRows = " << mParameterModel.rowCount();
}

void ModuleConfigurator::on_buttonBox_accepted() {
  qDebug() << "nRows = " << mParameterModel.rowCount() << " column"
           << mParameterModel.columnCount();
  for (int row = 0; row < mParameterModel.rowCount(); row++) {
    QString name;
    QVariant value;
    const auto nameItem = mParameterModel.item(row, 0);
    if (nameItem) {
      name = nameItem->data(Qt::DisplayRole).toString();
    } else {
      qWarning() << QString("empty item at %1:%2").arg(row).arg(2);
    }

    const auto valueItem = mParameterModel.item(row, 1);
    if (valueItem) {
      value = valueItem->data(Qt::DisplayRole);
    } else {
      qWarning() << QString("empty item at %1:%2").arg(row).arg(1);
    }
    mCurrentModule->setValue(name, value);
  }
  mParameterModel.clear();
}

void ModuleConfigurator::on_buttonBox_rejected() { mParameterModel.clear(); }
