#ifndef IMAGEPOINTSECTION_H
#define IMAGEPOINTSECTION_H

#include<QPoint>
#include<QJsonObject>
#include<QJsonArray>
class ImagePointSection
{
  static const QString centerString;
  QPoint center;
  void setCenter(const QJsonArray& array);
  QJsonArray centerAsArray() const;
public:
  ImagePointSection();
  ImagePointSection(const QPoint& center);
  ImagePointSection(const QJsonObject& obj);
  QJsonObject toJson() const;
  QPoint getCenter() const;
  bool isValid() const;
  void setCenter(const QPoint &value);
  bool operator ==(const ImagePointSection& other);
};

#endif // IMAGEPOINTSECTION_H
