#ifndef LABELEDIMAGESET_H
#define LABELEDIMAGESET_H

#include "labeledimage.h"
#include <QList>
#include <QDir>
#include <QTextStream>

class LabeledImageSet
{
  QList<LabeledImage> imageSet;
public:
  LabeledImageSet();
  void add(const LabeledImage& image);
  void add(const LabeledImageSet& set);
  const QList<LabeledImage>& getImageSet() const;
  void setImageSet(const QList<LabeledImage> &value);
  void save(const QDir& destination);
  void shuffle();
};

#endif // LABELEDIMAGESET_H
