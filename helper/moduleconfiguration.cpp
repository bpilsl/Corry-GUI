#include "moduleconfiguration.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonValue>
#include <QTextStream>

ModuleConfiguration::ModuleConfiguration(const QJsonObject &obj,
                                         QObject *parent)
    : QObject{parent} {
  initFromJson(obj);
}

ModuleConfiguration::ModuleConfiguration(const ModuleConfiguration &other,
                                         QObject *parent)
    : QObject(parent) {
  mName = other.name();
  foreach (const auto key, other.mParameters.keys()) {
    mParameters[key] = new Parameter(*other.mParameters[key]);
  }
}

void ModuleConfiguration::initFromJson(const QJsonObject &obj) {
  mName = obj["module_name"].toString();
  //  qDebug() << "\n\n module: " << mName;
  auto params = obj["parameters"];

  if (!params.isObject()) {
    qWarning() << "json parameters key is no object";
  }
  auto paraObj = params.toObject();
  for (const auto &key : paraObj.keys()) {
    auto value = paraObj[key];
    auto tmp = new Parameter(value.toVariant());
    mParameters[key] = tmp;
  }
}

QString ModuleConfiguration::toCorryConfigSection() {
  QString out;
  QTextStream cfg(&out);
  cfg << "[" << mName << "]\n";
  if (!mDetectorName.isEmpty()) {
    cfg << "name = " << mDetectorName << "\n";
  }
  if (!mDetectorType.isEmpty()) {
    cfg << "type = " << mDetectorType << "\n";
  }
  foreach (const auto &param, mParameters.keys()) {
    cfg << param2Str(param) << "\n";
  }
  cfg.flush();
  return out;
}
