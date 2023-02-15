#ifndef GEOMETRYBUILDERDIALOG_H
#define GEOMETRYBUILDERDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <QJsonArray>

namespace Ui {
class GeometryBuilder;
}

class GeometryBuilder : public QDialog {
  Q_OBJECT

public:
  struct Detector {
    constexpr static double sensorDrawThickness = 10.0;
    QString name, type, role;
    int position[3], orientation[3], nmbOfPixels[2];
    double pitch[2];
    QRectF grahicsRect;
    bool initialized;
    Detector(const QString &name, const QString &type, const QString &role,
             int pixX, int pixY, double pitchX, double pitchY, int posZ,
             int posX = 0, int posY = 0, int rotZ = 0, int rotX = 0,
             int rotY = 0) {
      this->name = name;
      this->type = type;
      this->role = role;
      nmbOfPixels[0] = pixX;
      nmbOfPixels[1] = pixY;
      pitch[0] = pitchX;
      pitch[1] = pitchY;
      position[0] = posX;
      position[1] = posY;
      position[2] = posZ;
      orientation[0] = rotX;
      orientation[1] = rotY;
      orientation[2] = rotZ;
      calcGraphics();
      initialized = true;
    }
    Detector() { initialized = false; }

    inline void calcGraphics() {
      grahicsRect.setTopLeft(QPoint(position[2], position[0]));
      grahicsRect.setWidth(sensorDrawThickness);
      grahicsRect.setHeight(pitch[0] * double(nmbOfPixels[0]) *
                            1e-3); // convert from um to mm
    }
    QString toCorryConfig() const;
    inline bool operator==(const Detector &other) const {
      return this->name == other.name && this->type == other.type;
    }
  };

  explicit GeometryBuilder(QWidget *parent = nullptr);
  inline auto scene() { return &mScene; }
  ~GeometryBuilder();

  QStringList availableDetectorNames();
  QStringList availableDetectorTypes();
  void paintGeometry();
  bool saveToCorryConfig(const QString &file);
  Detector &detectorAtPos(const QPointF &pos);
  void configureDetector(Detector &det);
  void deleteDetector(Detector &det);
  Detector *mDetector2Edit = nullptr;
  void import(const QList<Detector> &detectors);
  bool parseDetectorLib(const QString &file);

signals:
  void repainted();

private slots:
  void on_buttonBox_accepted();

  void on_cbDetLib_currentTextChanged(const QString &arg1);

private:
  struct LibEntry {
    int nPixel[2], pitch[2];
    QString type, name;
    LibEntry(int pixelX, int pixelY, int pitchX, int pitchY, const QString &n,
             const QString &t) {
      nPixel[0] = pixelX;
      nPixel[1] = pixelY;
      pitch[0] = pitchX;
      pitch[1] = pitchY;
      name = n;
      type = t;
    }
    LibEntry(){};
  };
  const double acceptPosTolerance = 5.0;

  void alignDetectors();

  Ui::GeometryBuilder *ui;
  QGraphicsScene mScene;
  QList<Detector> mDetectors;
  QMap<QString, LibEntry> mDetectorLib;
};

#endif // GEOMETRYBUILDERDIALOG_H
