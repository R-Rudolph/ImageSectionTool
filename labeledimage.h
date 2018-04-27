#ifndef LABELEDIMAGE_H
#define LABELEDIMAGE_H

#include<QImage>

class LabeledImage
{
  QImage image;
  int label;
public:
  LabeledImage();
  LabeledImage(const QImage& image, int label);
  int getLabel() const;
  void setLabel(int value);
  const QImage& getImage() const;
  void setImage(const QImage &value);
  void loadImage(const QString& filename);
  bool saveImage(const QString& filename) const;
  bool isValid() const;
};

#endif // LABELEDIMAGE_H
