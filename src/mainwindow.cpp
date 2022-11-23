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
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->lvModules->setModel(&mModulesModel);
  ui->lvModules->setDragEnabled(true);
  ui->lvConfig->setModel((&mConfigModel));
  ui->lvConfig->setDropIndicatorShown(true);
  ui->lvConfig->setAcceptDrops(true);
}

MainWindow::~MainWindow() { delete ui; }

bool MainWindow::parseAvailableModules(const QString &file) {
  QFile f(file);
  if (!f.open(QIODevice::ReadOnly)) {
    return false;
  }
  auto content = QString(f.readAll());
  auto jsonDocument = QJsonDocument::fromJson(content.toUtf8());

  if (!jsonDocument.isArray()) {
    qWarning() << "Given json file does not contain top level array";
    return false;
  }

  auto modules = jsonDocument.array();

  for (const auto &module : modules) {
    if (!module.isObject()) {
      qWarning() << "module description is no json object";
      continue;
    }
    auto config = new ModuleConfiguration(module.toObject(), this);
    mAvailableModules.append(config);
  }
  return true;
}

void MainWindow::on_pbLoad_clicked() {
  auto file = QFileDialog::getOpenFileName(this);
  if (!parseAvailableModules(file)) {
    qWarning() << "failed to parse file " << file;
  }

  mModulesModel.clear();
  int i = 0;
  foreach (const auto module, mAvailableModules) {
    auto item = new QStandardItem(module->name());
    item->setData(module->name(), Qt::DisplayRole);
    mModulesModel.setItem(i, item);
    i++;
  }
  mModulesModel.sort(0);
}

void MainWindow::on_pushButton_clicked() {
  auto idx = mModulesModel.index(0, 0);
  auto mime = mModulesModel.mimeData(QModelIndexList() << idx);
  qDebug() << mime->formats() << mime->text();
  mime->setText("I <3 Noussi");
  qDebug() << mime->formats() << mime->text();
}
