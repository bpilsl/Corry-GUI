#ifndef MODULECONFIGURATION_H
#define MODULECONFIGURATION_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QObject>
#include <QRegularExpression>
#include <QVariant>

class ModuleConfiguration : public QObject {
  Q_OBJECT
public:
  struct Parameter {
    QVariant value, defaultValue;

    Parameter(const QVariant &defaultVal) { defaultValue = defaultVal; }
  };

  ModuleConfiguration(const QJsonObject &obj, QObject *parent = nullptr);
  ModuleConfiguration(const ModuleConfiguration &other,
                      QObject *parent = nullptr);

  void initFromJson(const QJsonObject &obj);
  auto name() const { return mName; };
  auto defaultValue(const QString &module) const {
    return mParameters[module]->defaultValue;
  }
  auto setValue(const QString &module, const QVariant &val) {
    mParameters[module]->value = val;
  }
  auto parameters() const { return mParameters.keys(); }

private:
  QString mName;
  QMap<QString, Parameter *> mParameters;
};

#endif // MODULECONFIGURATION_H
