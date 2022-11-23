#include <QDataStream>
#include <QDebug>
#include <QFileSystemModel>

#include "corrymodulemodel.h"

CorryConfigModel::CorryConfigModel(QObject *parent)
    : QAbstractItemModel(parent) {}

QVariant CorryConfigModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const {
  // FIXME: Implement me!
}

QModelIndex CorryConfigModel::index(int row, int column,
                                    const QModelIndex &parent) const {
  // FIXME: Implement me!
}

QModelIndex CorryConfigModel::parent(const QModelIndex &index) const {
  // FIXME: Implement me!
}

int CorryConfigModel::rowCount(const QModelIndex &parent) const {
  if (!parent.isValid())
    return 0;

  // FIXME: Implement me!
}

int CorryConfigModel::columnCount(const QModelIndex &parent) const {
  if (!parent.isValid())
    return 0;

  // FIXME: Implement me!
}

QVariant CorryConfigModel::data(const QModelIndex &index, int role) const {
  qDebug() << " asking data of " << index.row();
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole) {
    return QVariant(mModules[index.row()].name());
  }
  return QVariant();
}

bool CorryConfigModel::insertRows(int row, int count,
                                  const QModelIndex &parent) {
  beginInsertRows(parent, row, row + count - 1);
  // FIXME: Implement me!
  endInsertRows();
  return true;
}

bool CorryConfigModel::insertColumns(int column, int count,
                                     const QModelIndex &parent) {
  beginInsertColumns(parent, column, column + count - 1);
  // FIXME: Implement me!
  endInsertColumns();
  return true;
}

bool CorryConfigModel::removeRows(int row, int count,
                                  const QModelIndex &parent) {
  beginRemoveRows(parent, row, row + count - 1);
  // FIXME: Implement me!
  endRemoveRows();
  return true;
}

bool CorryConfigModel::removeColumns(int column, int count,
                                     const QModelIndex &parent) {
  beginRemoveColumns(parent, column, column + count - 1);
  // FIXME: Implement me!
  endRemoveColumns();
  return true;
}

bool CorryConfigModel::dropMimeData(const QMimeData *data,
                                    Qt::DropAction action, int row, int column,
                                    const QModelIndex &parent) {
  const QString mimeType = "application/x-qstandarditemmodeldatalist";

  if (data->hasFormat(mimeType)) {
    QStandardItem item;
    decodeMimeData(data->data(mimeType), item);
    qDebug() << "dropped item " << item.data(0);
  }
}

Qt::DropActions CorryConfigModel::supportedDropActions() const {
  return Qt::CopyAction;
}

bool CorryConfigModel::canDropMimeData(const QMimeData *data,
                                       Qt::DropAction action, int row,
                                       int column,
                                       const QModelIndex &parent) const {
  return true;
}

Qt::ItemFlags CorryConfigModel::flags(const QModelIndex &index) const {
  auto defaultFlags =
      Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;

  //  if (index.isValid()) {
  //    return defaultFlags | Qt::ItemIsDropEnabled;
  //  }

  return defaultFlags;
}

void CorryConfigModel::decodeMimeData(const QByteArray &data,
                                      QStandardItem &item) {
  QDataStream ds(data);
  while (!ds.atEnd()) {
    qint32 row, col, map_items;
    ds >> row;
    ds >> col;
    ds >> map_items;

    for (int i = 0; i < map_items; i++) {
      qint32 role;
      QVariant val;
      ds >> role;
      ds >> val;
      item.setData(val, role);
    }
  }
}
