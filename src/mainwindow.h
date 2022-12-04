#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../helper/corrymodulemodel.h"
#include "geometrybuilder.h"
#include "moduleconfigurator.h"

#include <QMainWindow>
#include <QMenu>
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
  void exportToCfgClicked();
  void on_pbAdd_clicked();
  void on_pbMainConfig_clicked();
  void editModuleConfig();
  void editGeoConfig();
  void customMenuRequestedModuleConfig(const QPoint &pos);
  void customMenuRequestedGeometry(const QPoint &pos);
  void fitSceneToGv();
  void deleteModule();
  void deleteDetector();

private:
  Ui::MainWindow *ui;
  ModuleConfigurator *mModuleConfigurator;

  QStandardItemModel mModulesModel;
  CorryConfigModel mConfigModel;
  GeometryBuilder mGeometryBuilder;
  QMenu mMenuConfigList, mMenuGeo;
  QModelIndex mSelectedModule;
  GeometryBuilder::Detector *mSelectedDetector;
};
#endif // MAINWINDOW_H
