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
  connect(&mParameterModel, &QStandardItemModel::itemChanged, this,
          &ModuleConfigurator::markNonDefault);
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
  ui->cbName->clear();
  ui->cbType->clear();
  ui->cbName->addItem("");
  ui->cbName->addItems(mAvailableDetectorNames);
  ui->cbType->addItem("");
  ui->cbType->addItems(mAvailableDetectorTypes);

  ui->cbName->setCurrentText(config.detectorName());
  ui->cbType->setCurrentText(config.detectorType());

  mParameterModel.removeRows(0, mParameterModel.rowCount());

  foreach (const auto &param, config.parameters()) {
    QList<QStandardItem *> row;
    row.append(new QStandardItem(param));
    auto value = new QStandardItem();
    bool ok;
    auto nmb = config.value(param).toDouble(&ok);
    if (ok) {
      value->setData(nmb, Qt::EditRole);
    } else {
      value->setData(config.value(param), Qt::EditRole);
    }
    if (value->data(Qt::EditRole) != config.defaultValue(param)) {
      value->setForeground(QBrush(Qt::blue));
    }
    row << value;
    auto unit = new QStandardItem();
    unit->setData(config.unit(param), Qt::EditRole);
    row << unit;
    mParameterModel.appendRow(row);
  }
  ui->tvParameters->adjustSize();
}

void ModuleConfigurator::on_buttonBox_accepted() {
  mCurrentModule->setDetectorName(ui->cbName->currentText());
  mCurrentModule->setDetectorType(ui->cbType->currentText());
  for (int row = 0; row < mParameterModel.rowCount(); row++) {
    QString name, unit;
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
      mCurrentModule->setValue(name, value);
    } else {
      qWarning() << QString("empty item at %1:%2").arg(row).arg(1);
    }
    const auto unitItem = mParameterModel.item(row, 2);
    if (unitItem) {
      unit = unitItem->data(Qt::DisplayRole).toString();
      mCurrentModule->setUnit(name, unit);
    }
  }
}

void ModuleConfigurator::on_buttonBox_rejected() {}

void ModuleConfigurator::markNonDefault(QStandardItem *item) {
  auto idx = item->index();
  if (idx.column() == 1) {
    auto data = item->data(Qt::EditRole);
    auto defaultVal = mCurrentModule->defaultValue(
        mParameterModel.item(idx.row(), 0)->data(Qt::DisplayRole).toString());
    if (data != defaultVal) {
      item->setForeground(QBrush(Qt::blue));
    } else {
      auto defaultBrush = qApp->palette().windowText();
      item->setForeground(defaultBrush);
    }
  }
}
