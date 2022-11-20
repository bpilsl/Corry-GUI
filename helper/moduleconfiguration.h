#ifndef MODULECONFIGURATION_H
#define MODULECONFIGURATION_H

#include "qvariant.h"
#include <QObject>

class ModuleConfiguration : public QObject {
  Q_OBJECT
public:
  explicit ModuleConfiguration(const QString &descriptor,
                               QObject *parent = nullptr);

  void initFromDescription(const QString &descr);

signals:

private:
  struct Parameter {
    QString name;
    QVariant value, defaultValue;
  };

  QMap<QList, Parameter> mParameters;
  QString mName;
};

#endif // MODULECONFIGURATION_H
