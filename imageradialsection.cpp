#include "imageradialsection.h"
#include <QtMath>

const QString ImageRadialSection::centerString = "center";
const QString ImageRadialSection::radiusString = "radius";

QPointF ImageRadialSection::getCenter() const
{
  return center;
}

void ImageRadialSection::setCenter(const QPointF &value)
{
  center = value;
}

qreal ImageRadialSection::getRadius() const
{
  return radius;
}

void ImageRadialSection::setRadius(const qreal &value)
{
  radius = value;
}

void ImageRadialSection::setCenter(const QJsonArray &array)
{
  if(array.size()<2)
  {
    radius = 0;
  }
  else
  {
    center = QPointF(array[0].toDouble(),array[1].toDouble());
  }
}

QJsonArray ImageRadialSection::centerAsArray() const
{
  QJsonArray result;
  result.append(center.x());
  result.append(center.y());
  return result;
}

ImageRadialSection::ImageRadialSection()
{
  this->radius = 0;
}

ImageRadialSection::ImageRadialSection(const QPointF &center, const qreal &radius)
{
  this->center = center;
  this->radius = radius;
}

ImageRadialSection::ImageRadialSection(const QJsonObject &obj) : ImageRadialSection()
{
  setRadius(obj[radiusString].toDouble(0));
  setCenter(obj[centerString].toArray());
}

ImageRadialSection::ImageRadialSection(const QPointF &center, const QPointF &point)
{
  this->center = center;
  QPointF diff = center - point;
  radius = qSqrt(QPointF::dotProduct(diff,diff));
}

bool ImageRadialSection::isValid() const
{
  return radius > 0;
}

QJsonObject ImageRadialSection::toJson() const
{
  QJsonObject result;
  result.insert(centerString,centerAsArray());
  result.insert(radiusString,radius);
  return result;
}
