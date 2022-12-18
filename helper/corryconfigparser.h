#ifndef CORRYCONFIGPARSER_H
#define CORRYCONFIGPARSER_H

#include <QJsonArray>
#include <QObject>
#include <QRegularExpression>

#include "../src/geometrybuilder.h"

class CorryParser : public QObject {
  Q_OBJECT
public:
  explicit CorryParser(const QString &configFile, QObject *parent = nullptr);

  inline auto modules() { return mModules; }
  inline auto detectors() { return mDetectors; }

signals:

private:
  const QRegularExpression sectionIdentifier =
      QRegularExpression(R"(\[(.+)\])");
  const QRegularExpression vectorSeperator = QRegularExpression(R"(\d+)");

  bool fullParse(const QString &file);
  bool parseCorryConfig(const QString &file, QJsonArray &output);
  void constructDetectors(const QJsonArray &geometry);
  QList<double> extractNumbersFromVector(const QString &vector);

  QJsonArray mModules;
  QList<GeometryBuilder::Detector> mDetectors;
};

#endif // CORRYCONFIGPARSER_H
