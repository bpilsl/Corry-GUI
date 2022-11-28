#ifndef CORRYMODULEMODEL_H
#define CORRYMODULEMODEL_H

#include <QAbstractItemModel>
#include <QDropEvent>
#include <QMimeData>
#include <QStandardItem>

#include "../src/moduleconfigurator.h"
#include "moduleconfiguration.h"

class CorryConfigModel : public QAbstractListModel {
  Q_OBJECT

public:
  explicit CorryConfigModel(ModuleConfigurator *configurator,
                            QObject *parent = nullptr);

  // Header:
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  // Basic functionality:
  //  QModelIndex index(int row, int column,
  //                    const QModelIndex &parent = QModelIndex()) const
  //                    override;
  //  QModelIndex parent(const QModelIndex &index) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  //  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  // Add data:
  //  bool insertRows(int row, int count,
  //                  const QModelIndex &parent = QModelIndex()) override;
  //  bool insertColumns(int column, int count,
  //                     const QModelIndex &parent = QModelIndex()) override;

  // Remove data:
  //  bool removeRows(int row, int count,
  //                  const QModelIndex &parent = QModelIndex()) override;
  //  bool removeColumns(int column, int count,
  //                     const QModelIndex &parent = QModelIndex()) override;

  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row,
                    int column, const QModelIndex &parent) override;
  Qt::DropActions supportedDropActions() const override;
  bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row,
                       int column, const QModelIndex &parent) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  bool parseAvailableModules(const QString &file);
  auto availableModules() { return mAvailableModules; };
  bool exportToCfg(const QString &file);
  bool editItem(const QModelIndex &index);
  bool editGlobalCfg();
  QString detectorsFile();

private:
  const QString acceptableMimeType = "application/x-qstandarditemmodeldatalist";

  QList<ModuleConfiguration *> mModules;
  ModuleConfigurator *mConfigurator;
  QList<ModuleConfiguration *> mAvailableModules;
  ModuleConfiguration *mGlobalConfig = nullptr;

  bool decodeMimeData(const QMimeData &data, QStandardItem &item) const;
  bool canDecodeMime(const QMimeData &data) const;

private slots:
  void acceptConfiguredModule();
};

#endif // CORRYMODULEMODEL_H
