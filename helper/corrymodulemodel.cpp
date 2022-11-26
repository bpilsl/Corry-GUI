#include <QDataStream>
#include <QDebug>
#include <QFileSystemModel>

#include "corrymodulemodel.h"

CorryConfigModel::CorryConfigModel(ModuleConfigurator *configurator,
                                   QObject *parent)
    : QAbstractTableModel(parent), mConfigurator(configurator) {

  connect(mConfigurator, &ModuleConfigurator::accepted, this,
          &CorryConfigModel::acceptConfiguredModule);
}

QVariant CorryConfigModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const {
  qDebug() << "calling header data";
  return QVariant();
}

int CorryConfigModel::rowCount(const QModelIndex &parent) const {
  //  if (!parent.isValid())
  //    return 0;

  return mModules.length();
}

int CorryConfigModel::columnCount(const QModelIndex &parent) const {
  //  if (!parent.isValid())
  //    return 0;

  if (mModules.length() > 0) {
    return 1;
  } else {
    return 0;
  }
}

QVariant CorryConfigModel::data(const QModelIndex &index, int role) const {
  QVariant retval;
  qDebug() << " asking data of " << index.row() << " for role " << role;
  if (!index.isValid()) {
    return retval;
  }

  switch (role) {
  case Qt::DisplayRole:
    qDebug() << "returning " << QVariant(mModules[index.row()]->name());
    retval = QVariant(mModules[index.row()]->name());
    break;
    //  case Qt::DecorationRole:
    //    retval = QVariant(QColor("red"));
    //    break;
    //  case Qt::FontRole:
    //    retval = QVariant(QFont("Times", 20, 5));
    //    break;
    //  case Qt::TextAlignmentRole:
    //    retval = QVariant(Qt::AlignLeft);
    //    break;
    //  case Qt::ForegroundRole:
    //    retval = QVariant(QBrush(Qt::yellow));
    //    break;
    //  case Qt::BackgroundRole:
    //    retval = QVariant(QBrush(Qt::red));
    //    break;
    //  case Qt::SizeHintRole:
    //    retval = QVariant(QSize(100, 50));
    //    break;
  }
  qDebug() << "returning " << retval;
  return retval;
}

bool CorryConfigModel::dropMimeData(const QMimeData *data,
                                    Qt::DropAction action, int row, int column,
                                    const QModelIndex &parent) {
  const QString mimeType = "application/x-qstandarditemmodeldatalist";

  if (data->hasFormat(mimeType)) {
    QStandardItem item;
    if (!decodeMimeData(*data, item)) {
      return false;
    }
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
      qDebug() << "accepted " << config->name();
      mModules.append(config);
      auto currIdx = createIndex(mModules.length(), 0);
      emit dataChanged(currIdx, createIndex(currIdx.row(), 0));
      return true;
    } else {
      delete config;
    }
  }
  return false;
}

Qt::DropActions CorryConfigModel::supportedDropActions() const {
  return Qt::CopyAction;
}

bool CorryConfigModel::canDropMimeData(const QMimeData *data,
                                       Qt::DropAction action, int row,
                                       int column,
                                       const QModelIndex &parent) const {
  return canDecodeMime(*data);
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
bool CorryConfigModel::decodeMimeData(const QMimeData &data,
                                      QStandardItem &item) const {

  if (!canDecodeMime(data)) {
    return false;
  }
  QDataStream ds(data.data(acceptableMimeType));
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
  return true;
}

bool CorryConfigModel::canDecodeMime(const QMimeData &data) const {
  return data.hasFormat(acceptableMimeType);
}

void CorryConfigModel::acceptConfiguredModule() {}