#include "geometrybuilder.h"
#include "ui_geometrybuilder.h"

#include <QFile>
#include <QGraphicsItem>
#include <QTextStream>

GeometryBuilder::GeometryBuilder(QWidget *parent)
    : QDialog(parent), ui(new Ui::GeometryBuilder) {
  ui->setupUi(this);
  ui->cbRole->addItem("");
  ui->cbRole->addItem("dut");
  ui->cbRole->addItem("reference");
  ui->cbRole->addItem("auxiliary");
}

GeometryBuilder::~GeometryBuilder() {
  delete ui;
  foreach (auto detector, mDetectors) {
    delete detector;
  }
}

QStringList GeometryBuilder::availableDetectorNames() {
  QStringList retval;
  foreach (const auto &detector, mDetectors) {
    if (!retval.contains(detector->name)) {
      retval.append(detector->name);
    }
  }
  return retval;
}

QStringList GeometryBuilder::availableDetectorTypes() {
  QStringList retval;
  foreach (const auto &detector, mDetectors) {
    if (!retval.contains(detector->type)) {
      retval.append(detector->type);
    }
  }
  return retval;
}

void GeometryBuilder::paintGeometry() {

  const QList<QColor> colorMap = {
      QColor("black"), QColor("red"),    QColor("blue"),
      QColor("green"), QColor("yellow"), QColor("purple"),
      QColor("beige"), QColor("pink"),   QColor("orange")};
  mScene.clear();
  int maxDim[3] = {0};
  double maxHeight = 0.0;
  foreach (const auto &detector, mDetectors) {
    auto height = detector->grahicsRect.height();
    if (maxHeight < height) {
      maxHeight = height;
    }
    for (int i = 0; i < 3; i++) {
      if (maxDim[i] < detector->position[i]) {
        maxDim[i] = detector->position[i];
      }
    }
  }
  mScene.setSceneRect(QRectF(0.0, 0.0,
                             maxDim[2] + Detector::sensorDrawThickness,
                             maxDim[0] + maxHeight));

  /*
   * we paint in the x-z plane:
   *
   * ^x
   * |
   * |      z
   * |------>
   */
  alignDetectors();
  int i = 0;
  foreach (const auto &detector, mDetectors) {
    /* height of sensor => dimesion in scene y
     * position of sensor in z => start in scene x
     * width = arbitrary sensor thickness
     * align in x-plane like:
     *
     * + height / 2 -----------
     *
     * x-pos => 0   -----------
     *
     * -height /2   -----------
     */
    auto rect = mScene.addRect(detector->grahicsRect, QPen(),
                               QBrush(colorMap[i % colorMap.length()]));
    auto label = detector->name + ":\n" + detector->type;
    QFont font;
    // TODO: scale fontsize properly to scene contents (is either too big or too
    // small atm)
    font.setPointSize(2);
    auto text = mScene.addSimpleText(label, font);
    text->setPos(
        QPoint(detector->grahicsRect.left(), detector->grahicsRect.top() - 8));
    rect->setData(0, detector->name);
    i++;
  }
  emit repainted();
}

bool GeometryBuilder::saveToCorryConfig(const QString &file) {
  QFile f(file);
  if (!f.open(QIODevice::WriteOnly)) {
    return false;
  }
  QTextStream out(&f);
  foreach (const auto &detector, mDetectors) {
    out << detector->toCorryConfig() << "\n";
  }
  return true;
}

GeometryBuilder::Detector *GeometryBuilder::detectorAtPos(const QPointF &pos) {
  auto g = mScene.itemAt(pos, QTransform());
  if (g == nullptr) {
    return nullptr;
  }
  auto name = g->data(0).toString();

  foreach (auto det, mDetectors) {
    if (det->name == name)
      return det;
  }
  return nullptr;
}

void GeometryBuilder::configureDetector(Detector *det) {
  if (det == nullptr) {
    return;
  }
  mDetector2Edit = det;
  ui->leName->setText(det->name);
  ui->leType->setText(det->type);
  ui->cbRole->setCurrentText(det->role);
  ui->sbPixX->setValue(det->nmbOfPixels[0]);
  ui->sbPixY->setValue(det->nmbOfPixels[1]);
  ui->sbPitchX->setValue(det->pitch[0]);
  ui->sbPitchY->setValue(det->pitch[1]);
  ui->sbPosX->setValue(det->position[0]);
  ui->sbPosY->setValue(det->position[1]);
  ui->sbPosZ->setValue(det->position[2]);
  ui->sbRotX->setValue(det->orientation[0]);
  ui->sbRotY->setValue(det->orientation[1]);
  ui->sbRotZ->setValue(det->orientation[2]);
  this->exec();
  mDetector2Edit = nullptr;
}

void GeometryBuilder::deleteDetector(Detector *det) {
  mDetectors.removeAll(det);
  delete det;
  paintGeometry();
}

bool GeometryBuilder::import(const QJsonArray &config) {
  qDebug() << "geo import";
}

void GeometryBuilder::alignDetectors() {
  double maxHeight = 0.0;
  foreach (const auto &det, mDetectors) {
    if (det->grahicsRect.height() > maxHeight) {
      maxHeight = det->grahicsRect.height();
    }
  }
  foreach (auto &det, mDetectors) {
    det->grahicsRect.moveTop(
        (maxHeight - det->grahicsRect.height() + double(det->position[0])) /
        2.0);
  }
}

void GeometryBuilder::on_buttonBox_accepted() {
  auto detector = mDetector2Edit;
  if (detector == nullptr) {
    detector = new Detector(
        ui->leName->text(), ui->leType->text(), ui->cbRole->currentText(),
        ui->sbPixX->value(), ui->sbPixY->value(), ui->sbPitchX->value(),
        ui->sbPitchY->value(), ui->sbPosZ->value(), ui->sbPosX->value(),
        ui->sbPosY->value(), ui->sbRotZ->value(), ui->sbRotX->value(),
        ui->sbRotY->value());
    mDetectors.append(detector);
  } else {
    detector->name = ui->leName->text();
    detector->type = ui->leType->text();
    detector->role = ui->cbRole->currentText();
    detector->nmbOfPixels[0] = ui->sbPixX->value();
    detector->nmbOfPixels[1] = ui->sbPixY->value();
    detector->pitch[0] = ui->sbPitchX->value();
    detector->pitch[1] = ui->sbPitchY->value();
    detector->position[0] = ui->sbPosX->value();
    detector->position[1] = ui->sbPosY->value();
    detector->position[2] = ui->sbPosZ->value();
    detector->orientation[0] = ui->sbRotX->value();
    detector->orientation[1] = ui->sbRotY->value();
    detector->orientation[2] = ui->sbRotZ->value();
    detector->calcGraphics();
  }
  paintGeometry();
}

QString GeometryBuilder::Detector::toCorryConfig() {
  QString out;
  QTextStream cfg(&out);
  cfg << "[" << name << "]\n";
  if (!role.isEmpty()) {
    cfg << "role = " << role << "\n";
  }
  cfg << "type = " << type << "\n";
  cfg << "pixel_pitch = " << pitch[0] << "um, " << pitch[1] << "um\n";
  cfg << "position = " << position[0] << "mm, " << position[1] << "mm, "
      << position[2] << "mm\n";
  cfg << "orientation = " << orientation[0] << "deg, " << orientation[1]
      << "deg, " << orientation[2] << "deg\n";

  cfg.flush();
  return out;
}
