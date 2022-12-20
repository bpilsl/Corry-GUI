#include "runmanager.h"
#include "ui_runmanager.h"

#include <QDir>
#include <QMessageBox>

RunManager::RunManager(QWidget *parent)
    : QWidget(parent), ui(new Ui::RunManager) {
  ui->setupUi(this);
  connect(&mCorry, &QProcess::readyReadStandardOutput, this,
          &RunManager::processCorryOutput);
  connect(&mCorry, &QProcess::readyReadStandardError, this,
          &RunManager::processCorryOutput);

  configure();
}

RunManager::~RunManager() {
  delete ui;
  mCorry.kill();
}

void RunManager::configure() {
  mCorry.setProgram("~/development/corryvreckan/bin/corry");
}

void RunManager::updateEventLoaders(
    const QList<const ModuleConfiguration *> eventLoaders) {
  ui->twEventLoaders->clearContents();
  for (int i = 0; i < eventLoaders.length(); i++) {
    ui->twEventLoaders->insertRow(i);
    const auto loader = eventLoaders[i];
    ui->twEventLoaders->item(i, 0)->setText(loader->name());
    auto detectorId = loader->detectorType() + ":" + loader->detectorName();
    ui->twEventLoaders->item(i, 1)->setText(detectorId);
  }
}

void RunManager::on_pbStart_clicked() {

  if (mCorry.state() != QProcess::NotRunning) {
    qWarning() << "Corry already running";
    return;
  }
  QDir workingDir(ui->leDir->text());
  QString configFile = ui->leConfig->text();
  auto deployConfig = QMessageBox::question(
      this, "deploy config?", "Save current config before executing Corry?");
  if (deployConfig == QMessageBox::Ok) {
    emit requestConfigDeploy(workingDir.path(), configFile);
  } else {
  }
  QStringList args;

  args << "-c " + workingDir.absoluteFilePath(configFile);

  if (ui->sbRun->value() >=
      0) { // provide filename for eventloader as command line argument to corry
           // if run number specified, if not use file in config file
    for (int i = 0; i < ui->twEventLoaders->rowCount(); i++) {
      auto moduleName = ui->twEventLoaders->item(i, 0)->text();
      auto detectorName = ui->twEventLoaders->item(i, 1)->text();
      QString filenameOption = moduleName + ":" + detectorName + ".file_name=";
      auto wildcard = ui->twEventLoaders->item(i, 2)->text();
      int digitCount = wildcard.count("$");

      /*
       * replace the placeholder for the run number in the filename by the run
       * number specified in ui eg: UI filename "*run<xxx>*.raw" will be
       * replaced by "*run069*.raw" the so generated filename matches any file
       * considering the given wildcard, eg:
       * "mimosa26_03081992_run069.raw"
       * "dut_run069_175620221204.raw"
       * "tlu_run069_13061994.raw"
       */
      wildcard.replace(
          QRegularExpression("<x+>"),
          QString("%1").arg(ui->sbRun->value(), digitCount, 10, QChar('0')));

      auto dataFile =
          workingDir.entryInfoList(QStringList() << wildcard, QDir::Files);
      if (dataFile.length() != 1) {
        qDebug() << "not exactly one file matching specified wildcard";
        return;
      }
      args << "-o " + filenameOption + dataFile[0].fileName();
    }
  }

  mCorry.setArguments(args);
  mCorry.setWorkingDirectory(workingDir.path());
  mCorry.start();
}

void RunManager::processCorryOutput() {
  ui->tbCorryOutput->append(mCorry.readAllStandardOutput());
}

void RunManager::on_pbClear_clicked() { ui->tbCorryOutput->clear(); }

void RunManager::on_pbStop_clicked() { mCorry.terminate(); }
