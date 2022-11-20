#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
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
  auto modules = content.split(QRegularExpression(".+[.+].+"));

  foreach (const auto &module, modules) {
    ModuleConfiguration config(module);
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
