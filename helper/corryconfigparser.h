#ifndef CORRYCONFIGPARSER_H
#define CORRYCONFIGPARSER_H

#include <QJsonArray>
#include <QObject>
#include <QRegularExpression>

class CorryParser : public QObject {
  Q_OBJECT
public:
  explicit CorryParser(const QString &configFile, QObject *parent = nullptr);

  inline auto geometry() { return mGeometry; }
  inline auto modules() { return mModules; }

signals:

private:
  const QRegularExpression sectionIdentifier =
      QRegularExpression(R"(\[(.+)\])");

  bool fullParse(const QString &file);
  bool parseCorryConfig(const QString &file, QJsonArray &output);

  QJsonArray mModules;
  QJsonArray mGeometry;
};

#endif // CORRYCONFIGPARSER_H
