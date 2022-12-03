#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QMenu>

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

  ui->lvConfig->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->lvConfig, &QListView::customContextMenuRequested, this,
          &MainWindow::customMenuRequestedModuleConfig);
  ui->gvDetectorSetup->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->gvDetectorSetup, &QGraphicsView::customContextMenuRequested, this,
          &MainWindow::customMenuRequestedGeometry);
  auto editActionModuleConfig = new QAction("edit", this);
  connect(editActionModuleConfig, &QAction::triggered, this,
          &MainWindow::editModuleConfig);
  mMenuConfigList.addAction(editActionModuleConfig);
  auto editActionGeoDetector = new QAction("edit", this);
  connect(editActionGeoDetector, &QAction::triggered, this,
          &MainWindow::editGeoConfig);
  mMenuGeo.addAction(editActionGeoDetector);

  connect(&mGeometryBuilder, &GeometryBuilder::repainted, this,
          &MainWindow::fitSceneToGv);

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

void MainWindow::customMenuRequestedModuleConfig(const QPoint &pos) {
  QModelIndex index = ui->lvConfig->indexAt(pos);
  if (!index.isValid()) {
    return;
  }
  mSelectedModule = index;
  mMenuConfigList.popup(ui->lvConfig->viewport()->mapToGlobal(pos));
}

void MainWindow::customMenuRequestedGeometry(const QPoint &pos) {
  auto mapped = ui->gvDetectorSetup->mapToScene(pos);
  mSelectedDetector = mGeometryBuilder.detectorAtPos(mapped);
  if (mSelectedDetector == nullptr) {
    return;
  }
  mMenuGeo.popup(ui->gvDetectorSetup->viewport()->mapToGlobal(pos));
}

void MainWindow::fitSceneToGv() {
  ui->gvDetectorSetup->fitInView(mGeometryBuilder.scene()->sceneRect(),
                                 Qt::KeepAspectRatio);
}

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
  auto detectorsFile = file.remove(QRegularExpression(R"([\w-]+\..+)")) +
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

void MainWindow::editModuleConfig() { mConfigModel.editItem(mSelectedModule); }

void MainWindow::editGeoConfig() {
  mGeometryBuilder.configureDetector(mSelectedDetector);
}
