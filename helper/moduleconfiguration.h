#ifndef MODULECONFIGURATION_H
#define MODULECONFIGURATION_H

#include "qvariant.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QObject>
#include <QRegularExpression>

class ModuleConfiguration : public QObject {
  Q_OBJECT
public:
  explicit ModuleConfiguration(const QJsonObject &obj,
                               QObject *parent = nullptr);

  void initFromJson(const QJsonObject &obj);

signals:

private:
  struct Parameter {
    QVariant value, defaultValue;

    Parameter(const QVariant &defaultVal) { defaultValue = defaultVal; }
  };

  QString mName;
  QMap<QString, Parameter *> mParameters;
};

#endif // MODULECONFIGURATION_H
