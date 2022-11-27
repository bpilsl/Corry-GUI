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

void GeometryBuilder::on_buttonBox_accepted() {
  auto detector = new Detector(
      ui->leName->text(), ui->leType->text(), ui->cbRole->currentText(),
      ui->sbPixX->value(), ui->sbPixY->value(), ui->sbPosZ->value(),
      ui->sbPosX->value(), ui->sbPosY->value(), ui->sbRotZ->value(),
      ui->sbRotX->value(), ui->sbRotY->value());
  mDetectors.append(detector);
}

QString GeometryBuilder::Detector::toCorryConfig() {
  QString out;
  QTextStream cfg(&out);
  cfg << "[" << name << "]\n";
  cfg << "role = " << role << "\n";
  cfg << "type = " << type << "\n";
  cfg << "position = " << position[0] << "mm, " << position[1] << "mm, "
      << position[2] << "mm\n";
  cfg << "orientation = " << rotation[0] << "deg, " << rotation[1] << "deg, "
      << rotation[2] << "deg\n";

  cfg.flush();
  return out;
}
