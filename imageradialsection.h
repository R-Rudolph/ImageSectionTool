#ifndef IMAGERADIALSECTION_H
#define IMAGERADIALSECTION_H

#include <QPointF>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

class ImageRadialSection
{
  static const QString radiusString;
  static const QString centerString;
  QPointF center;
  qreal radius;
  void setCenter(const QJsonArray& array);
  QJsonArray centerAsArray() const;
public:
  ImageRadialSection();
  ImageRadialSection(const QPointF& center, const qreal& radius);
  ImageRadialSection(const QJsonObject& obj);
  ImageRadialSection(const QPointF& center, const QPointF& point);
  bool isValid() const;
  QJsonObject toJson() const;
  QPointF getCenter() const;
  void setCenter(const QPointF &value);
  qreal getRadius() const;
  void setRadius(const qreal &value);
};

#endif // IMAGERADIALSECTION_H
