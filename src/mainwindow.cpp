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
      mConfigModel(mModuleConfigurator) {
  ui->setupUi(this);
  ui->lvModules->setModel(&mModulesModel);
  ui->lvModules->setDragEnabled(true);
  ui->lvConfig->setModel((&mConfigModel));
  ui->lvConfig->setDropIndicatorShown(true);
  ui->lvConfig->setAcceptDrops(true);

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

void MainWindow::on_pushButton_clicked() { ui->lvConfig->update(); }
