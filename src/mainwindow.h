#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../helper/corrymodulemodel.h"
#include "moduleconfigurator.h"

#include <QMainWindow>
#include <QStandardItemModel>

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
  void on_pushButton_clicked();
  void exportToCfgClicked();

private:
  Ui::MainWindow *ui;
  ModuleConfigurator *mModuleConfigurator;

  QStandardItemModel mModulesModel;
  CorryConfigModel mConfigModel;
};
#endif // MAINWINDOW_H
