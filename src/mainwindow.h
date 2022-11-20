#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "moduleconfigurator.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void on_pbLoad_clicked();

private:
  bool parseAvailableModules(const QString &file);

  Ui::MainWindow *ui;
  QList<ModuleConfiguration> mAvailableModules;
};
#endif // MAINWINDOW_H
