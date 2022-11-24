#include <QDataStream>
#include <QDebug>
#include <QFileSystemModel>

#include "corrymodulemodel.h"

CorryConfigModel::CorryConfigModel(ModuleConfigurator *configurator,
                                   QObject *parent)
    : QAbstractItemModel(parent), mConfigurator(configurator) {

  connect(mConfigurator, &ModuleConfigurator::accepted, this,
          &CorryConfigModel::acceptConfiguredModule);
}

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
    return QVariant(mModules[index.row()]->name());
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
    auto chosenModule = item.data(0).toString();
    ModuleConfiguration *config = nullptr;
    foreach (auto module, mAvailableModules) {
      if (module->name() == chosenModule) {
        config = new ModuleConfiguration(*module, this);
        break;
      }
    }
    if (config == nullptr) {
      return false;
    }
    auto accepted = mConfigurator->startConfiguration(*config);
    if (accepted) {
      mModules.append(config);
    } else {
      delete config;
    }
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

bool CorryConfigModel::parseAvailableModules(const QString &file) {
  QFile f(file);
  if (!f.open(QIODevice::ReadOnly)) {
    return false;
  }
  auto content = QString(f.readAll());
  auto jsonDocument = QJsonDocument::fromJson(content.toUtf8());

  if (!jsonDocument.isArray()) {
    qWarning() << "Given json file does not contain top level array";
    return false;
  }

  auto modules = jsonDocument.array();

  for (const auto &module : modules) {
    if (!module.isObject()) {
      qWarning() << "module description is no json object";
      continue;
    }
    auto config = new ModuleConfiguration(module.toObject(), this);
    mAvailableModules.append(config);
  }
  return true;
}

ModuleConfiguration CorryConfigModel::moduleDefaultConfig(const QString &name) {
  foreach (auto module, mAvailableModules) {
    if (module->name() == name) {
      return *module;
    }
  }
}
/**
 * @brief CorryConfigModel::decodeMimeData decode qt internal MIME data dropped
 * to us from an external QStandardItemModel
 * @param data the encoded MIME data
 * @param item StandardItem to store the decoded data into
 *
 * for details see
 * https://wiki.python.org/moin/PyQt/Handling%20Qt%27s%20internal%20item%20MIME%20type
 */
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

void CorryConfigModel::acceptConfiguredModule() {}
