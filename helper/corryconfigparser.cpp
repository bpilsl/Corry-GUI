#include "corryconfigparser.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QRegularExpression>

CorryParser::CorryParser(const QString &configFile, QObject *parent)
    : QObject{parent} {

  if (!fullParse(configFile)) {
    qWarning() << "error parsing config file";
  }
}

bool CorryParser::fullParse(const QString &file) {
  if (!parseCorryConfig(file, mModules)) {
    return false;
  }
  QJsonObject main;
  for (const auto &obj : qAsConst(mModules)) {
    if (!obj.isObject()) {
      continue;
    }
    auto tmp = obj.toObject();
    if (tmp["section_name"] == "Corryvreckan") {
      main = tmp;
      break;
    }
  }
  if (main.isEmpty()) {
    qDebug() << "No main config section found";
    return false;
  }

  // get and parse detectors file
  auto detectorsFile = main["detectors_file"].toString();
  detectorsFile =
      QFileInfo(file).path() + "/" +
      detectorsFile; // add path of main config file to detector file
  if (!parseCorryConfig(detectorsFile, mGeometry)) {
    return false;
  }
  return true;
}

bool CorryParser::parseCorryConfig(const QString &file, QJsonArray &output) {
  QFile f(file);
  if (!f.open(QIODevice::ReadOnly)) {
    return false;
  }
  QJsonObject currModule;
  while (!f.atEnd()) {
    auto line = f.readLine();
    if (line.isEmpty() || line.startsWith("#") || line.length() < 3) {
      continue;
    }
    auto match = sectionIdentifier.match(line);
    if (match.hasMatch()) { // new section
      if (!currModule.isEmpty()) {
        output << currModule;       // write previously built section to output
        currModule = QJsonObject(); // new object
      }
      currModule.insert("section_name", QJsonValue(match.captured(1)));
      continue;
    }

    // key and value are seperated by a '='
    auto split = line.split('=');
    if (split.length() != 2) {
      qDebug() << "invalid line in config file: " << line;
      continue;
    }
    auto key = split[0].trimmed();
    QJsonValue value = QString(split[1]).trimmed();
    currModule.insert(key, value);
  }
  return true;
}
