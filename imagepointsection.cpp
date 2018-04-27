#include "imagepointsection.h"

const QString ImagePointSection::centerString = "c";

QPoint ImagePointSection::getCenter() const
{
  return center;
}

bool ImagePointSection::isValid() const
{
  return center.x()>=0 && center.y()>=0;
}

void ImagePointSection::setCenter(const QPoint &value)
{
  center = value;
}

bool ImagePointSection::operator ==(const ImagePointSection &other)
{
  return center == other.center;
}

void ImagePointSection::setCenter(const QJsonArray &array)
{
  if(array.size()==2)
  {
    center = QPoint(array[0].toInt(-1),array[1].toInt(-1));
  }
}

QJsonArray ImagePointSection::centerAsArray() const
{
  QJsonArray result;
  result.append(center.x());
  result.append(center.y());
  return result;
}

ImagePointSection::ImagePointSection()
{
  center.rx() = -1;
  center.ry() = -1;
}

ImagePointSection::ImagePointSection(const QPoint &center)
{
  this->center = center;
}

ImagePointSection::ImagePointSection(const QJsonObject &obj) : ImagePointSection()
{
  setCenter(obj[centerString].toArray());
}

QJsonObject ImagePointSection::toJson() const
{
  QJsonObject result;
  result.insert(centerString,centerAsArray());
  return result;
}
