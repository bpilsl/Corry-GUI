#include "mainwindow.h"
#include "../helper/corryconfigparser.h"
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
  connect(ui->actionImport, &QAction::triggered, this,
          &MainWindow::importCfgClicked);
  connect(ui->actionLoad_GUI_Config, &QAction::triggered, this,
          &MainWindow::processGuiConfig);
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
  auto deleteActionModuleConfig = new QAction("delete", this);
  connect(deleteActionModuleConfig, &QAction::triggered, this,
          &MainWindow::deleteModule);
  mMenuConfigList.addAction(deleteActionModuleConfig);

  auto editActionGeoDetector = new QAction("edit", this);
  connect(editActionGeoDetector, &QAction::triggered, this,
          &MainWindow::editGeoConfig);
  mMenuGeo.addAction(editActionGeoDetector);

  auto deleteActionDetector = new QAction("delete", this);
  connect(deleteActionDetector, &QAction::triggered, this,
          &MainWindow::deleteDetector);
  mMenuGeo.addAction(deleteActionDetector);

  connect(&mGeometryBuilder, &GeometryBuilder::repainted, this,
          &MainWindow::fitSceneToGv);

  connect(ui->tabRun, &RunManager::updateEventLoaders, &mConfigModel,
          &CorryConfigModel::eventLoadersChanged);
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
  mSelectedDetector = &mGeometryBuilder.detectorAtPos(mapped);
  if (mSelectedDetector == nullptr) {
    return;
  }
  mMenuGeo.popup(ui->gvDetectorSetup->viewport()->mapToGlobal(pos));
}

void MainWindow::fitSceneToGv() {
  ui->gvDetectorSetup->fitInView(mGeometryBuilder.scene()->sceneRect(),
                                 Qt::KeepAspectRatio);
}

void MainWindow::deleteModule() {
  mConfigModel.removeRow(mSelectedModule.row());
}

void MainWindow::deleteDetector() {
  mGeometryBuilder.deleteDetector(*mSelectedDetector);
}

void MainWindow::processGuiConfig() {
  auto file = QFileDialog::getOpenFileName(this);
  QFile f(file);
  if (!f.open(QIODevice::ReadOnly)) {
    return;
  }
  auto jsonDocument = QJsonDocument::fromJson(f.readAll());
  if (!jsonDocument.isObject()) {
    return;
  }

  auto conf = jsonDocument.object();
  auto availableModules = conf["availableModules"].toString();
  auto availableDetectors = conf["availableDetectors"].toString();

  if (mConfigModel.parseAvailableModules(availableModules)) {
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
  if (!mGeometryBuilder.parseDetectorLib(availableDetectors)) {
    qWarning() << "error importing detector lib";
  }
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

void MainWindow::importCfgClicked() {
  auto file = QFileDialog::getOpenFileName(
      this, "Import config from (main Corry config, no geo)");
  if (file.isEmpty()) {
    return;
  }
  //  auto parser = CorryParser(file, this);
  CorryParser parser(file, this);
  mGeometryBuilder.import(parser.detectors());
  mModuleConfigurator->setAvailableDetectorNames(
      mGeometryBuilder.availableDetectorNames());
  mModuleConfigurator->setAvailableDetectorTypes(
      mGeometryBuilder.availableDetectorTypes());
  mConfigModel.import(parser.modules());
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
  mGeometryBuilder.configureDetector(*mSelectedDetector);
}
