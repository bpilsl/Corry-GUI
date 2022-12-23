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
  Parameter *tmp;
  auto paraObj = params.toObject();
  for (const auto &key : paraObj.keys()) {
    auto defaultValue = paraObj[key];
    if (defaultValue.isObject()) {
      auto valWithUnit = defaultValue.toObject();
      tmp = new Parameter(valWithUnit["value"].toVariant(),
                          valWithUnit["unit"].toString());
    } else {
      tmp = new Parameter(defaultValue.toVariant());
    }
    mParameters[key] = tmp;
  }
}

void ModuleConfiguration::configureFromImport(const QJsonObject &obj) {
  foreach (const auto &par, obj.keys()) {
    auto val = obj[par];
    if (par == "section_name") {
      continue;
    }
    if (par == "name") {
      mDetectorName = val.toString();
      continue;
    }
    if (par == "type") {
      mDetectorType = val.toString();
      continue;
    }
    if (!unit(par).isEmpty()) {
      // default value comes with unit, this means the parsed value most likely
      // also has got a unit
      auto number = numberExtractor.match(val.toString());
      if (number.hasMatch()) {
        setValue(par, number.captured(0).toDouble());
      }
      auto unit = unitExtractor.match(val.toString());
      if (unit.hasMatch()) {
        setUnit(par, unit.captured(0));
      }
      continue;
    }
    setValue(par, obj[par]);
  }
}

QString ModuleConfiguration::toCorryConfigSection(bool printDefault) {
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

    if (printDefault) {
      cfg << param2Str(param) << "\n";
    } else if (!mParameters[param]->isDefault()) {
      // only print non default values
      cfg << param2Str(param) << "\n";
    }
  }
  cfg.flush();
  return out;
}

QString ModuleConfiguration::param2Str(const QString &key) {
  QString out;
  QTextStream ts(&out);

  if (unit(key).isEmpty()) {
    ts << key << " = " << value(key).toString();
  } else {
    ts << key << " = " << value(key).toString() << unit(key);
  }
  ts.flush();
  return out;
}
