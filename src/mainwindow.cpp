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
    qDebug() << "It is not a JSON array";
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
}
