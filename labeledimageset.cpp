#include "labeledimageset.h"

const QList<LabeledImage>& LabeledImageSet::getImageSet() const
{
  return imageSet;
}

void LabeledImageSet::setImageSet(const QList<LabeledImage> &value)
{
  imageSet = value;
}

void LabeledImageSet::save(const QDir &destination)
{
  QFile labelFile(destination.absoluteFilePath("labels.txt"));
  if(labelFile.open(QIODevice::WriteOnly))
  {
    QTextStream stream(&labelFile);
    int ctr = 0;
    foreach(const LabeledImage& image, imageSet)
    {
      QString filename = QString::number(ctr)+".png";
      QString filepath = destination.absoluteFilePath(filename);
      image.saveImage(filepath);
      stream << filepath << " " << image.getLabel() << endl;
      ctr++;
    }
    labelFile.close();
  }
}

void LabeledImageSet::shuffle()
{
  for(int i=0;i<imageSet.size();i++)
  {
    qSwap(imageSet[i],imageSet[qrand()%imageSet.size()]);
  }
}

LabeledImageSet::LabeledImageSet()
{

}

void LabeledImageSet::add(const LabeledImage &image)
{
  imageSet.append(image);
}

void LabeledImageSet::add(const LabeledImageSet &set)
{
  const QList<LabeledImage>& list = set.getImageSet();
  foreach(const LabeledImage& image, list)
    add(image);
}
