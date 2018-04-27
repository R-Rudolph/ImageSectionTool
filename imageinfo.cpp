#include "imageinfo.h"
#include <QPixmap>

const QString ImageInfo::radSectionString = "radSections";
const QString ImageInfo::pointSectionString = "pointSections";
const QString ImageInfo::testDataString = "testdata";
const QString ImageInfo::ignoreString = "ignore";

QString ImageInfo::getFilepath() const
{
  return filepath;
}

void ImageInfo::setFilepath(const QString &value)
{
  filepath = value;
}

QList<const ImageSection *> ImageInfo::getSections()
{
  QList<const ImageSection *> result;
  for(int i=0;i<sections.size();i++)
    result.append(&(sections[i]));
  return result;
}

QList<const ImageRadialSection *> ImageInfo::getRadialSections()
{
  QList<const ImageRadialSection *> result;
  for(int i=0;i<radSections.size();i++)
    result.append(&(radSections[i]));
  return result;
}

void ImageInfo::addImagePointSection(const ImagePointSection &section)
{
  pointSections.append(section);
}

void ImageInfo::removeImagePointSection(const ImagePointSection &section)
{
  pointSections.removeAll(section);
}

QList<const ImagePointSection *> ImageInfo::getPointSections() const
{
  QList<const ImagePointSection *> result;
  for(int i=0;i<pointSections.size();i++)
    result.append(&(pointSections[i]));
  return result;
}

void ImageInfo::addImageSection(const ImageSection &section)
{
  sections.append(section);
}

void ImageInfo::removeImageSection(const ImageSection &section)
{
  for(int i=0;i<sections.size();i++)
  {
    if(sections[i].getPolygon()==section.getPolygon())
    {
      sections.removeAt(i);
      i--;
    }
  }
}

void ImageInfo::addImageRadialSection(const ImageRadialSection &section)
{
  radSections.append(section);
}

void ImageInfo::removeImageRadialSection(const ImageRadialSection &section)
{
  for(int i=0;i<radSections.size();i++)
  {
    if(radSections[i].getCenter()==section.getCenter())
    {
      radSections.removeAt(i);
      i--;
    }
  }
}

LabeledImageSet ImageInfo::generateLabeledSet(int height, int width) const
{
  QMap<int,int> objectOccurance;
  QImage image(filepath);
  LabeledImageSet set;
  int maxOccurance = 0;
  //sample from section, count images of each class
  foreach(const ImageSection& section, sections)
  {
    LabeledImageSet subset = sampleFromSection(image,section,height,width);
    int classNum = section.getClassNum();
    if(!objectOccurance.contains(classNum))
    {
      objectOccurance.insert(classNum,0);
    }
    objectOccurance[classNum] += subset.getImageSet().size();
    maxOccurance = qMax(maxOccurance,objectOccurance[classNum]);
    set.add(subset);
  }
  //extract background
  QList<QRect> extracted;
  QRect imageRect = image.rect();
  QPoint center = imageRect.center();
  imageRect.setWidth(imageRect.width()-width);
  imageRect.setHeight(imageRect.height()-height);
  imageRect.moveCenter(center);
  /*
  int offset[imageRect.height()];
  for(int i=0;i<imageRect.height();i++)
    offset[i] = width/2;
  */
  for(int y=imageRect.top();y<imageRect.bottom();y++)
  {
    for(int x=imageRect.left();x<imageRect.right();x++)
    {
      QRect rect(0,0,width,height);
      rect.moveCenter(QPoint(x,y));
      bool cont = false;
      for(int i=0;i<extracted.size();i++)
      {
        int right = extracted[i].right();
        if((right>x) && extracted[i].intersects(rect))
        {
          cont = true;
          x = extracted[i].right()+1;
          break;
        }
      }
      if(cont)
        continue;
      for(int i=0;i<sections.size();i++)
      {
        int right = sections[i].getPolygon().boundingRect().right();
        if(sections[i].intersects(rect))
        {
          cont = true;
          if(right>x)
            x = right;
          break;
        }
      }
      if(cont)
        continue;
      extracted.append(rect);
      x+=width-1;
    }
  }
  //remove selected background parts
  while(extracted.size()>maxOccurance*3)
  {
    extracted.removeAt(qrand()%extracted.size());
  }
  //extract background images
  foreach(QRect rect, extracted)
  {
    QImage imagePart = image.copy(rect);
    set.add(LabeledImage(imagePart,0));
  }
  return set;
}

void ImageInfo::saveSegmentationImage(QString destPath) const
{
  if(destPath.isEmpty())
  {
    destPath = filepath + ".png";
  }
  QPixmap image(filepath,nullptr,Qt::MonoOnly);
  image.fill(Qt::black);
  foreach(const ImageSection& section, sections)
  {
    section.fill(image);
  }
  image.save(destPath);
}

void ImageInfo::saveSegmentationImageSegNet(QString destPath) const
{
  if(destPath.isEmpty())
  {
    destPath = filepath + ".seg.png";
  }
  QPixmap image(filepath,nullptr,Qt::MonoOnly);
  image.fill(Qt::black);
  foreach(const ImageSection& section, sections)
  {
    section.fill(image,QColor(64,64,64));
  }
  image.save(destPath);
}

QString ImageInfo::getFilename() const
{
  return getFilepath().split("/").last();
}

QList<std::pair<QPixmap, QPixmap> > ImageInfo::getImageSegmentations(int size) const
{
  QList<std::pair<QPixmap, QPixmap> > result;
  QPixmap image(filepath);
  QPixmap segmentation(filepath,nullptr,Qt::MonoOnly);
  segmentation.fill(Qt::black);
  foreach(const ImageSection& section, sections)
  {
    section.fill(segmentation,QColor(1,1,1));
  }
  for(int x=0;x<=image.width()-size;x+=size)
    for(int y=0;y<=image.height()-size;y+=size)
    {
      QRect rect(x,y,size,size);
      result.append(std::pair<QPixmap, QPixmap>(image.copy(rect),
                                                segmentation.copy(rect)));
    }
  /*
  int sizeX = image.width()/parts;
  int sizeY = image.height()/parts;
  for(int i=0;i<parts;i++)
    for(int j=0;j<parts;j++)
    {
      QRect rect(i*sizeX,j*sizeY,sizeX,sizeY);
      result.append(std::pair<QPixmap, QPixmap>(image.copy(rect),
                                                segmentation.copy(rect)));

    }
  */
  return result;
}

QPixmap ImageInfo::getImageSegmentation() const
{
  QPixmap segmentation(filepath,nullptr,Qt::MonoOnly);
  segmentation.fill(Qt::black);
  foreach(const ImageSection& section, sections)
  {
    section.fill(segmentation,QColor(1,1,1));
  }
  return segmentation;
}

bool ImageInfo::isInTestSet() const
{
  return isTestData() && !getIgnore();
}

bool ImageInfo::isInTrainingSet() const
{
  return isTrainingData() && !getIgnore();
}

bool ImageInfo::isTrainingData() const
{
  return !isTestData();
}

bool ImageInfo::isTestData() const
{
  return testData;
}

void ImageInfo::setTestData(bool value)
{
  testData = value;
}

bool ImageInfo::getIgnore() const
{
  return ignore;
}

void ImageInfo::setIgnore(bool value)
{
  this->ignore = value;
}

bool ImageInfo::imageExists() const
{
  return QFile(filepath).exists();
}

bool ImageInfo::updateFiledirectory(const QDir &dir)
{
  QStringList filepathParts = filepath.split('/');
  QString newFilepath;
  for(int i = filepathParts.size();i --> 0;)
  {
    if(!newFilepath.isEmpty())
      newFilepath = "/"+newFilepath;
    newFilepath = dir.absoluteFilePath(filepathParts[i] + newFilepath);
    if(QFile(newFilepath).exists())
    {
      filepath = newFilepath;
      return true;
    }
  }
  return false;
}

LabeledImageSet ImageInfo::sampleFromSection(const QImage& image, const ImageSection &section, int height, int width) const
{
  QRect bbox = section.getPolygon().boundingRect();
  QPoint center = bbox.center();
  bbox.setWidth(bbox.width()-width);
  bbox.setHeight(bbox.height()-height);
  bbox.moveCenter(center);
  if(!bbox.isValid())
  {
    if(bbox.width()<0)
      bbox.setWidth(0);
    if(bbox.height()<0)
      bbox.setHeight(0);
  }
  QPolygon pol = section.getPolygon();
  LabeledImageSet set;
  for(int x=bbox.left();x<bbox.right();x++)
    for(int y=bbox.top();y<bbox.bottom();y++)
    {
      QPoint p(x,y);
      if(pol.containsPoint(p,Qt::OddEvenFill))
      {
        QRect rect(0,0,width,height);
        rect.moveCenter(p);
        QImage imagePart = image.copy(rect);
        set.add(LabeledImage(imagePart,section.getClassNum()+1));
        pol = pol.subtracted(QPolygon(rect));
      }
    }
  return set;
}

ImageInfo::ImageInfo(const QString &filepath)
{
  ignore = false;
  testData = false;
  this->filepath = filepath;
}

ImageInfo::ImageInfo(const QJsonObject &json)
{
  this->ignore = json[ignoreString].toBool(false);
  this->filepath = json["filepath"].toString();
  this->testData = json[testDataString].toBool(false);
  {
    QJsonArray sectionsArray = json["sections"].toArray();
    for(int i=0;i<sectionsArray.size();i++)
    {
      ImageSection sec(sectionsArray[i].toObject());
      if(!sec.isEmpty())
        sections.append(sec);
    }
  }
  {
    QJsonArray radSectionArray = json[radSectionString].toArray();
    for(int i=0;i<radSectionArray.size();i++)
    {
      ImageRadialSection radSec(radSectionArray[i].toObject());
      if(radSec.isValid())
      {
        radSections.append(radSec);
      }
    }
  }
  {
    QJsonArray pointSectionArray = json[pointSectionString].toArray();
    for(int i=0;i<pointSectionArray.size();i++)
    {
      ImagePointSection pointSec(pointSectionArray[i].toObject());
      if(pointSec.isValid())
      {
        pointSections.append(pointSec);
      }
    }
  }
}

QJsonObject ImageInfo::toJson()
{
  QJsonObject result;
  result.insert("filepath",filepath);
  result.insert(testDataString,isTestData());
  result.insert(ignoreString,getIgnore());
  {
    QJsonArray sectionsArray;
    for(int i=0;i<sections.size();i++)
      sectionsArray.append(sections[i].toJson());
    result.insert("sections",sectionsArray);
  }
  {
    QJsonArray radSectionsArray;
    for(int i=0;i<radSections.size();i++)
    {
      if(radSections[i].isValid())
        radSectionsArray.append(radSections[i].toJson());
    }
    result.insert(radSectionString,radSectionsArray);
  }
  {
    QJsonArray pointSectionsArray;
    foreach(const ImagePointSection& section, pointSections)
      pointSectionsArray.append(section.toJson());
    result.insert(pointSectionString,pointSectionsArray);
  }
  return result;
}
