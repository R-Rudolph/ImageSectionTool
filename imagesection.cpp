#include "imagesection.h"
#include <QPainter>

QJsonArray ImageSection::jsonFromPolygon(const QPolygon &polygon)
{
  QJsonArray result;
  for(int i=0;i<polygon.size();i++)
  {
    QJsonObject point;
    point.insert("x",polygon[i].x());
    point.insert("y",polygon[i].y());
    result.append(point);
  }
  return result;
}

QPolygon ImageSection::polygonFromJson(const QJsonArray &json)
{
  QPolygon polygon;
  for(int i=0;i<json.size();i++)
  {
    QJsonObject pointObj = json[i].toObject();
    QPoint point(pointObj["x"].toInt(),pointObj["y"].toInt());
    polygon.append(point);
  }
  return polygon;
}

ImageSection::ImageSection()
{

}

ImageSection::ImageSection(const QJsonObject &json)
{
  polygon = polygonFromJson(json["rect"].toArray());
  classNum = json["class"].toInt();
}

ImageSection::ImageSection(const QPolygon &polygon, int classNum)
{
  this->polygon = polygon;
  this->classNum = classNum;
}

QPolygon ImageSection::getPolygon() const
{
  return polygon;
}

void ImageSection::setPolygon(const QPolygon &value)
{
  polygon = value;
}

int ImageSection::getClassNum() const
{
  return classNum;
}

void ImageSection::setClassNum(int value)
{
  classNum = value;
}

QJsonObject ImageSection::toJson()
{
  QJsonObject result;
  result.insert("rect",jsonFromPolygon(polygon));
  result.insert("class",classNum);
  return result;
}

bool ImageSection::isEmpty()
{
  return polygon.size()<3;
}

bool ImageSection::intersects(const QRect &rect) const
{
  QPolygon pol = polygon.intersected(QPolygon(rect));
  return !pol.isEmpty();
}

void ImageSection::fill(QPixmap &image, const QColor& col) const
{
  QPainter painter(&image);
  painter.setBrush(col);
  painter.setPen(col);
  painter.drawPolygon(polygon);
}
