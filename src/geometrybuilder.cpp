#include "geometrybuilder.h"
#include "ui_geometrybuilder.h"

#include <QFile>
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
  const double sensorThickness = 10.0;
  const QList<QColor> colorMap = {
      QColor("black"), QColor("red"),    QColor("blue"),
      QColor("green"), QColor("yellow"), QColor("purple"),
      QColor("beige"), QColor("pink"),   QColor("orange")};
  mScene.clear();
  int maxDim[3] = {0};
  double maxHeight = 0.0;
  foreach (const auto &detector, mDetectors) {
    auto height = detector->pitch[0] * double(detector->nmbOfPixels[0]) * 1e-3;
    if (maxHeight < height) {
      maxHeight = height;
    }
    for (int i = 0; i < 3; i++) {
      if (maxDim[i] < detector->position[i]) {
        maxDim[i] = detector->position[i];
      }
    }
  }
  mScene.setSceneRect(QRectF(0.0, 0.0, maxDim[2], maxDim[0] + maxHeight));

  /*
   * we paint in the x-z plane:
   *
   * ^x
   * |
   * |      z
   * |______>
   */
  int i = 0;
  foreach (const auto &detector, mDetectors) {
    // scene x corresponds to our z position and y to our x
    auto height = detector->pitch[0] * double(detector->nmbOfPixels[0]) /
                  (mScene.sceneRect().height());
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

    mScene.addRect(
        QRectF(detector->position[2],
               (maxHeight - height + double(detector->position[0])) / 2.0,
               sensorThickness, height),
        QPen(), QBrush(colorMap[i % colorMap.length()]));
    i++;
  }
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

void GeometryBuilder::on_buttonBox_accepted() {
  auto detector = new Detector(
      ui->leName->text(), ui->leType->text(), ui->cbRole->currentText(),
      ui->sbPixX->value(), ui->sbPixY->value(), ui->sbPitchX->value(),
      ui->sbPitchY->value(), ui->sbPosZ->value(), ui->sbPosX->value(),
      ui->sbPosY->value(), ui->sbRotZ->value(), ui->sbRotX->value(),
      ui->sbRotY->value());
  mDetectors.append(detector);
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
