#ifndef MODULECONFIGURATION_H
#define MODULECONFIGURATION_H

#include <QDebug>
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
    // TODO: value and default value might differ in terms of unit
    //  isDefault function, parsing,...
    //  maybe introduce struct for values with value and unit
    QVariant value, defaultValue;
    QString unit;

    Parameter(const QVariant &defaultVal) { defaultValue = defaultVal; }
    Parameter(const QVariant &defaultVal, const QString &unit) {
      defaultValue = defaultVal;
      this->unit = unit;
    }
    auto isDefault() { return !(value.isValid() && value != defaultValue); }
  };

  ModuleConfiguration(const QJsonObject &obj, QObject *parent = nullptr);
  ModuleConfiguration(const ModuleConfiguration &other,
                      QObject *parent = nullptr);

  void initFromJson(const QJsonObject &obj);
  void configureFromImport(const QJsonObject &obj);
  inline auto name() const { return mName; };
  inline auto defaultValue(const QString &key) const {
    return mParameters[key]->defaultValue;
  }
  inline auto setValue(const QString &key, const QVariant &val) {
    if (!mParameters.contains(key)) {
      qWarning() << "trying to set key " << key << " for module " << mName
                 << " which is not available!";
      return;
    }
    mParameters[key]->value = val;
  }
  inline auto setUnit(const QString &key, const QString &unit) {
    if (!mParameters.contains(key)) {
      qWarning() << "trying to set key " << key << " for module " << mName
                 << " which is not available!";
      return;
    }
    mParameters[key]->unit = unit;
  }

  inline auto setDetectorName(const QString &name) { mDetectorName = name; }
  inline auto setDetectorType(const QString &type) { mDetectorType = type; }
  inline auto detectorName() const { return mDetectorName; }
  inline auto detectorType() const { return mDetectorType; }
  inline auto value(const QString &key) {
    if (mParameters[key]->value.isValid()) {
      return mParameters[key]->value;
    }
    return mParameters[key]->defaultValue;
  }
  inline auto unit(const QString &key) const { return mParameters[key]->unit; }
  inline auto isEventLoader() const { return mName.startsWith("EventLoader"); }

  auto parameters() const { return mParameters.keys(); }
  QString toCorryConfigSection(bool printDefault = false);

private:
  const QRegularExpression numberExtractor = QRegularExpression(R"(\d+)");
  const QRegularExpression unitExtractor = QRegularExpression(R"(\D+)");

  QString mName;
  QString mDetectorName, mDetectorType;
  QMap<QString, Parameter *> mParameters;

  QString param2Str(const QString &key);
};

#endif // MODULECONFIGURATION_H
