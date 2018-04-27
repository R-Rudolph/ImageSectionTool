#include "labeledimage.h"

int LabeledImage::getLabel() const
{
  return label;
}

void LabeledImage::setLabel(int value)
{
  label = value;
}

const QImage& LabeledImage::getImage() const
{
  return image;
}

void LabeledImage::setImage(const QImage &value)
{
  image = value;
}

void LabeledImage::loadImage(const QString &filename)
{
  image.load(filename);
}

bool LabeledImage::saveImage(const QString &filename) const
{
  if(isValid())
    return image.save(filename);
  else
    return false;
}

bool LabeledImage::isValid() const
{
  return !image.isNull();
}

LabeledImage::LabeledImage()
{
  label = 0;
}

LabeledImage::LabeledImage(const QImage &image, int label)
{
  this->image = image;
  this->label = label;
}
