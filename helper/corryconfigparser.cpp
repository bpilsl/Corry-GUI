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
  QJsonArray geometry;
  if (!parseCorryConfig(detectorsFile, geometry)) {
    return false;
  }
  constructDetectors(geometry);
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

void CorryParser::constructDetectors(const QJsonArray &geometry) {
  mDetectors.clear();
  for (const auto &det : qAsConst(geometry)) {
    if (!det.isObject()) {
      qDebug() << "Array element is no json object, skipping";
      continue;
    }
    auto obj = det.toObject();
    auto name = obj["section_name"].toString();
    auto role = obj["role"].toString();
    auto type = obj["type"].toString();

    auto tmp = obj["pixel_pitch"].toString();
    double pitch[2] = {-1};
    auto pi = extractNumbersFromVector(tmp);
    if (pi.length() == 2) {
      pitch[0] = pi[0];
      pitch[1] = pi[1];
    } else {
      qWarning() << "invalid number of entries in: " << tmp;
    }

    tmp = obj["position"].toString();
    double pos[3] = {-1.0};
    auto p = extractNumbersFromVector(tmp);
    if (p.length() == 3) {
      pos[0] = p[0];
      pos[1] = p[1];
      pos[2] = p[2];
    } else {
      qWarning() << "invalid number of entries in: " << tmp;
    }

    tmp = obj["orientation"].toString();
    double orientation[3] = {-1.0};
    auto o = extractNumbersFromVector(tmp);
    if (o.length() == 3) {
      orientation[0] = o[0];
      orientation[1] = o[1];
      orientation[2] = o[2];
    } else {
      qWarning() << "invalid number of entries in: " << tmp;
    }

    tmp = obj["number_of_pixels"].toString();
    int nmbPixels[2] = {-1};
    auto n = extractNumbersFromVector(tmp);
    if (n.length() == 2) {
      nmbPixels[0] = n[0];
      nmbPixels[1] = n[1];
    } else {
      qWarning() << "invalid number of entries in: " << tmp;
    }

    GeometryBuilder::Detector detector(
        name, type, role, nmbPixels[0], nmbPixels[1], pitch[0], pitch[1],
        pos[2], pos[0], pos[1], orientation[2], orientation[0], orientation[1]);
    mDetectors << detector;
  }
}

QList<double> CorryParser::extractNumbersFromVector(const QString &vector) {
  QList<double> retval;
  auto i = vectorSeperator.globalMatch(vector);
  while (i.hasNext()) {
    bool ok;
    auto match = i.next();
    retval.append(match.captured(0).toDouble(&ok));
    if (!ok) {
      qWarning() << "error converting vector to doubles: " << vector;
    }
  }
  return retval;
}
