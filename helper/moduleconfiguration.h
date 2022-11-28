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
  inline auto value(const QString &key) {
    if (mParameters[key]->value.isValid()) {
      return mParameters[key]->value;
    }
    return mParameters[key]->defaultValue;
  }

  auto parameters() const { return mParameters.keys(); }
  QString toCorryConfigSection();

private:
  QString mName;
  QMap<QString, Parameter *> mParameters;

  inline auto param2Str(const QString &key) {
    QString out;
    QTextStream ts(&out);

    ts << key << " = " << value(key).toString();
    ts.flush();
    return out;
  }
};

#endif // MODULECONFIGURATION_H
