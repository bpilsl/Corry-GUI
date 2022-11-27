#include "geometrybuilder.h"
#include "ui_geometrybuilder.h"

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
  int maxDim[3] = {0};
  foreach (const auto &detector, mDetectors) {
    for (int i = 0; i < 3; i++) {
      if (maxDim[i] < detector->position[i]) {
        maxDim[i] = detector->position[i];
      }
    }
  }
  mScene.setSceneRect(QRectF(0.0, 0.0, maxDim[0], maxDim[1]));
  int i = 0;
  foreach (const auto &detector, mDetectors) {
    mScene.addRect(QRectF(detector->position[2], detector->position[0], 10,
                          detector->pitch[0] * detector->nmbOfPixels[0]),
                   QPen(), QBrush(colorMap[i % colorMap.length()]));
    i++;
  }
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
  cfg << "role = " << role << "\n";
  cfg << "type = " << type << "\n";
  cfg << "pixel_pitch = " << pitch[0] << "um, " << pitch[1] << "um\n";
  cfg << "position = " << position[0] << "mm, " << position[1] << "mm, "
      << position[2] << "mm\n";
  cfg << "orientation = " << orientation[0] << "deg, " << orientation[1]
      << "deg, " << orientation[2] << "deg\n";

  cfg.flush();
  return out;
}
