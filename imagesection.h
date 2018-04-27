#ifndef IMAGESECTION_H
#define IMAGESECTION_H

#include <QRect>
#include <QPolygon>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QColor>

class QPixmap;

class ImageSection
{
  QPolygon polygon;
  int classNum;
  QJsonArray jsonFromPolygon(const QPolygon& polygon);
  QPolygon polygonFromJson(const QJsonArray& json);
public:
  ImageSection();
  ImageSection(const QJsonObject& json);
  ImageSection(const QPolygon& polygon, int classNum);
  QPolygon getPolygon() const;
  void setPolygon(const QPolygon &value);
  int getClassNum() const;
  void setClassNum(int value);
  QJsonObject toJson();
  bool isEmpty();
  bool intersects(const QRect& rect) const;
  void fill(QPixmap& image, const QColor& col=QColor(255,255,255)) const;
};

#endif // IMAGESECTION_H
