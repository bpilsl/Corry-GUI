#ifndef RUNMANAGER_H
#define RUNMANAGER_H

#include <QJsonObject>
#include <QProcess>
#include <QWidget>

#include "../helper/moduleconfiguration.h"

namespace Ui {
class RunManager;
}

class RunManager : public QWidget {
  Q_OBJECT

public:
  explicit RunManager(QWidget *parent = nullptr);
  ~RunManager();
  void configure();
public slots:
  void updateEventLoaders(const QList<const ModuleConfiguration *> eventLoaders);
signals:
  void requestConfigDeploy(const QString &path, const QString &file);
private slots:
  void on_pbStart_clicked();
  void on_pbClear_clicked();
  void processCorryOutput();

  void on_pbStop_clicked();

private:
  Ui::RunManager *ui;
  QProcess mCorry;
};

#endif // RUNMANAGER_H
