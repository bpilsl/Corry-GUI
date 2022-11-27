#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      mModuleConfigurator(new ModuleConfigurator(this)),
      mConfigModel(mModuleConfigurator), mGeometryBuilder(this) {
  ui->setupUi(this);
  ui->lvModules->setModel(&mModulesModel);
  ui->lvModules->setDragEnabled(true);
  ui->lvConfig->setModel((&mConfigModel));
  ui->lvConfig->setDropIndicatorShown(true);
  ui->lvConfig->setAcceptDrops(true);

  connect(ui->actionExport, &QAction::triggered, this,
          &MainWindow::exportToCfgClicked);
  ui->gvDetectorSetup->setScene(mGeometryBuilder.scene());

  mConfigModel.parseAvailableModules("modules.json");
  mModulesModel.clear();
  int i = 0;
  foreach (const auto module, mConfigModel.availableModules()) {
    auto item = new QStandardItem(module->name());
    item->setData(module->name(), Qt::DisplayRole);
    mModulesModel.setItem(i, item);
    i++;
  }
  mModulesModel.sort(0);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_pbLoad_clicked() {
  auto file = QFileDialog::getOpenFileName(this);
  if (!mConfigModel.parseAvailableModules(file)) {
    qWarning() << "failed to parse file " << file;
  }

  mModulesModel.clear();
  int i = 0;
  foreach (const auto module, mConfigModel.availableModules()) {
    auto item = new QStandardItem(module->name());
    item->setData(module->name(), Qt::DisplayRole);
    mModulesModel.setItem(i, item);
    i++;
  }
  mModulesModel.sort(0);
}

void MainWindow::exportToCfgClicked() {
  auto file = QFileDialog::getSaveFileName(this, "Export config to");
  if (file.isEmpty()) {
    return;
  }

  if (!mConfigModel.exportToCfg(file)) {
    qWarning() << "Error exporting config to file " << file;
  }

  // regexp removes file name
  auto detectorsFile = file.remove(QRegularExpression("[\\w-]+\\..+")) +
                       mConfigModel.detectorsFile();
  // extract path from user input
  if (!mGeometryBuilder.saveToCorryConfig(detectorsFile)) {
    qWarning() << "Error exporting geometry file "
               << mConfigModel.detectorsFile();
  }
}

void MainWindow::on_pbAdd_clicked() {
  auto result = mGeometryBuilder.exec();
  if (result == QDialog::Accepted) {
    mModuleConfigurator->setAvailableDetectorNames(
        mGeometryBuilder.availableDetectorNames());
    mModuleConfigurator->setAvailableDetectorTypes(
        mGeometryBuilder.availableDetectorTypes());
  }
}

void MainWindow::on_pbMainConfig_clicked() { mConfigModel.editGlobalCfg(); }
