#include "moduleconfiguration.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonValue>

ModuleConfiguration::ModuleConfiguration(const QJsonObject &obj,
                                         QObject *parent)
    : QObject{parent} {
  initFromJson(obj);
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
