#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include "imagesection.h"
#include "labeledimageset.h"
#include "imagepointsection.h"
#include <imageradialsection.h>
#include <QPixmap>

class ImageInfo
{
  static const QString radSectionString;
  static const QString pointSectionString;
  static const QString testDataString;
  static const QString ignoreString;
  bool testData;
  bool ignore;
  QString filepath;
  QList<ImageSection> sections;
  QList<ImageRadialSection> radSections;
  QList<ImagePointSection> pointSections;
  LabeledImageSet sampleFromSection(const QImage &image, const ImageSection& section, int height, int width) const;
public:
  ImageInfo(const QString& filepath = QString());
  ImageInfo(const QJsonObject& json);
  QJsonObject toJson();
  QString getFilepath() const;
  void setFilepath(const QString &value);

  void addImageSection(const ImageSection &section);
  void removeImageSection(const ImageSection &section);
  QList<const ImageSection *> getSections();

  void addImageRadialSection(const ImageRadialSection& section);
  void removeImageRadialSection(const ImageRadialSection& section);
  QList<const ImageRadialSection *> getRadialSections();

  void addImagePointSection(const ImagePointSection& section);
  void removeImagePointSection(const ImagePointSection& section);
  QList<const ImagePointSection*> getPointSections() const;

  LabeledImageSet generateLabeledSet(int height, int width) const;
  void saveSegmentationImage(QString destPath="") const;
  void saveSegmentationImageSegNet(QString destPath="") const;
  QString getFilename() const;
  QList< std::pair<QPixmap, QPixmap> > getImageSegmentations(int size) const;
  QPixmap getImageSegmentation() const;
  bool isInTestSet() const;
  bool isInTrainingSet() const;
  bool isTrainingData() const;
  bool isTestData() const;
  void setTestData(bool value);
  bool getIgnore() const;
  void setIgnore(bool value);
  bool imageExists() const;
  bool updateFiledirectory(const QDir& dir);
};

#endif // IMAGEINFO_H
