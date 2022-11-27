#ifndef GEOMETRYBUILDERDIALOG_H
#define GEOMETRYBUILDERDIALOG_H

#include <QDialog>
#include <QGraphicsScene>

namespace Ui {
class GeometryBuilder;
}

class GeometryBuilder : public QDialog {
  Q_OBJECT

public:
  struct Detector {
    QString name, type, role;
    int position[3], rotation[3], nmbOfPixels[2];
    Detector(const QString &name, const QString &type, const QString &role,
             int pixX, int pixY, int posZ, int posX = 0, int posY = 0,
             int rotZ = 0, int rotX = 0, int rotY = 0) {
      this->name = name;
      this->type = type;
      this->role = role;
      nmbOfPixels[0] = pixX;
      nmbOfPixels[1] = pixY;
      position[0] = posX;
      position[1] = posY;
      position[2] = posZ;
      rotation[0] = rotX;
      rotation[1] = rotY;
      rotation[2] = rotZ;
    }
    QString toCorryConfig();
  };

  explicit GeometryBuilder(QWidget *parent = nullptr);
  inline auto scene() { return &mScene; }
  ~GeometryBuilder();

private slots:
  void on_buttonBox_accepted();

private:
  Ui::GeometryBuilder *ui;
  QGraphicsScene mScene;
  QList<Detector *> mDetectors;
};

#endif // GEOMETRYBUILDERDIALOG_H
