#include "imagelibrary.h"

ImageInfo *ImageLibrary::infoByFilename(const QString &filename)
{
  for(int i=0;i<images.size();i++)
  {
    if(images[i].getFilepath()==filename)
      return &(images[i]);
  }
  return nullptr;
}

bool ImageLibrary::saveFileList(const QList<ImageInfo *> &list, const QString &filename)
{
  QFile file(filename);
  if(file.open(QIODevice::WriteOnly))
  {
    QTextStream stream(&file);
    foreach(ImageInfo* info, list)
    {
      stream << info->getFilepath() << endl;
    }
    file.close();
    return true;
  }
  return false;
}

ImageLibrary::ImageLibrary()
{

}

ImageLibrary::LoadResult ImageLibrary::load(const QString &libraryFilepath)
{
  QFile file(libraryFilepath);
  if(file.open(QIODevice::ReadOnly))
  {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(),&error);
    file.close();
    if(error.error != QJsonParseError::NoError)
      return ImageLibrary::LOAD_PARSEFAIL;
    QJsonArray fileArray = doc.array();
    ImageLibrary::LoadResult result = LOAD_SUCCESS;
    for(int i=0;i<fileArray.size();i++)
    {
      images.append(ImageInfo(fileArray[i].toObject()));
      if(!images.last().imageExists())
      {
        result = LOAD_IMAGEMISSING;
      }
    }
    return result;
  }
  else
  {
    return ImageLibrary::LOAD_OPENFAIL;
  }
}

bool ImageLibrary::save(const QString &libraryFilepath)
{
  QFile file(libraryFilepath);
  if(file.open(QIODevice::WriteOnly))
  {
    QJsonArray fileArray;
    for(int i=0;i<images.size();i++)
    {
      fileArray.append(images[i].toJson());
    }
    QJsonDocument doc(fileArray);
    file.write(doc.toJson());
    file.close();
    return true;
  }
  else
    return false;
}

void ImageLibrary::addImage(const QString &filename)
{
  if(infoByFilename(filename)==nullptr)
    images.append(ImageInfo(filename));
}

void ImageLibrary::addImages(const QStringList &filenames)
{
  for(int i=0;i<filenames.size();i++)
  {
    addImage(filenames[i]);
  }
}

QStringList ImageLibrary::getFileNames()
{
  QStringList list;
  for(int i=0;i<images.size();i++)
    list.append(images[i].getFilepath());
  return list;
}

QList<const ImageSection *> ImageLibrary::getSections(const QString &filename)
{
  QList<const ImageSection*> sections;
  ImageInfo* info = infoByFilename(filename);
  if(info!=nullptr)
  {
    sections = info->getSections();
  }
  return sections;
}

const QList<ImageInfo> &ImageLibrary::getInfos()
{
  return images;
}

void ImageLibrary::removeImageSection(const QString &filename, const ImageSection &section)
{
  ImageInfo* info = infoByFilename(filename);
  if(info!=nullptr)
  {
    info->removeImageSection(section);
  }
}

void ImageLibrary::addImageSection(const QString &filename, const ImageSection &section)
{
  ImageInfo* info = infoByFilename(filename);
  if(info!=nullptr)
  {
    info->addImageSection(section);
  }
}

void ImageLibrary::removeImageRadSection(const QString &filename, const ImageRadialSection &section)
{
  ImageInfo* info = infoByFilename(filename);
  if(info!=nullptr)
  {
    info->removeImageRadialSection(section);
  }
}

void ImageLibrary::addImageRadSection(const QString &filename, const ImageRadialSection &section)
{
  ImageInfo* info = infoByFilename(filename);
  if(info!=nullptr)
  {
    info->addImageRadialSection(section);
  }
}

void ImageLibrary::removeImagePointSection(const QString &filename, const ImagePointSection &section)
{
  ImageInfo* info = infoByFilename(filename);
  if(info!=nullptr)
  {
    info->removeImagePointSection(section);
  }
}

void ImageLibrary::addImagePointSection(const QString &filename, const ImagePointSection &section)
{
  ImageInfo* info = infoByFilename(filename);
  if(info!=nullptr)
  {
    info->addImagePointSection(section);
  }
}

void ImageLibrary::generateClassificationData(int width, int height, const QDir& dir)
{
  int ctr=0;
  QList<ImageInfo*> infoTrain, infoTest;
  for(int i=0;i<images.size();i++)
  {
    if(images[i].isInTestSet())
    {
      infoTest.append(&images[i]);
    }
    else if(images[i].isInTrainingSet())
    {
      infoTrain.append(&images[i]);
    }
  }
  {
    LabeledImageSet set;
    QDir dirTrain(dir.absoluteFilePath("train"));
    dirTrain.mkpath(dirTrain.absolutePath());
    foreach(const ImageInfo* info, infoTrain)
    {
      ctr++;
      set.add(info->generateLabeledSet(width,height));
    }
    set.shuffle();
    saveFileList(infoTrain,dir.absoluteFilePath("train.txt"));
    set.save(dirTrain);
  }

  {
    LabeledImageSet set;
    QDir dirTest(dir.absoluteFilePath("test"));
    dirTest.mkpath(dirTest.absolutePath());
    foreach(const ImageInfo* info, infoTest)
    {
      ctr++;
      set.add(info->generateLabeledSet(width,height));
    }
    set.shuffle();
    saveFileList(infoTest,dir.absoluteFilePath("test.txt"));
    set.save(dirTest);
  }
}

void ImageLibrary::saveSegmentationImages(QDir &dest)
{
  foreach(const ImageInfo& info, images)
  {
    if(info.isInTestSet())
      info.saveSegmentationImage(dest.absoluteFilePath(info.getFilename()+".png"));
  }
}

void ImageLibrary::saveSegmentationImagesSegNet(QDir &dest)
{
  QList<ImageInfo> trainImages = images;
  QList<ImageInfo> testImages;
  for(int i=0;i<images.size()/10;i++)
  {
    int index = qrand()%images.size();
    testImages.append(trainImages[index]);
    trainImages.removeAt(index);
  }
  writeList(dest,"train.txt",trainImages);
  writeList(dest,"test.txt",testImages);
  saveSegmentationImagesSetNet(dest,"train",trainImages);
  saveSegmentationImagesSetNet(dest,"test",testImages);
}

void ImageLibrary::saveSegmentationImagesJaccardIndex(QDir &dest)
{
  for(int i=0;i<images.size();i++)
  {
    images[i].getImageSegmentation().save(dest.absoluteFilePath(images[i].getFilename()+".png"));
  }
}

void ImageLibrary::writeList(const QDir &dest, const QString &filename, const QList<ImageInfo> &infos)
{
  QFile file(dest.absoluteFilePath(filename));
  file.open(QIODevice::WriteOnly);
  QTextStream t(&file);
  foreach(const ImageInfo& info, infos)
  {
    t << info.getFilename() << "\n";
  }
  file.close();
}

void ImageLibrary::saveSegmentationImagesSetNet(QDir &dest, const QString &text, const QList<ImageInfo> &infos)
{
  QDir subDir(dest.absoluteFilePath(text));
  QDir imagesDir(subDir.absoluteFilePath("images"));
  QDir labelsDir(subDir.absoluteFilePath("labels"));
  QDir().mkpath(dest.absoluteFilePath(text)+"/images");
  QDir().mkpath(dest.absoluteFilePath(text)+"/labels");
  QFile listFile(subDir.absoluteFilePath("data.txt"));
  listFile.open(QIODevice::WriteOnly);
  QTextStream ts(&listFile);
  int ctr = 0;
  for(int i=0;i<infos.size();i++)
  {
    QList<std::pair<QPixmap, QPixmap> > images = infos[i].getImageSegmentations(608);
    for(int j=0;j<images.size();j++)
    {
      QString imagePath = imagesDir.absoluteFilePath(QString::number(ctr)+".png");
      QString segmentationPath = labelsDir.absoluteFilePath(QString::number(ctr)+".seg.png");
      ctr++;
      images[j].first.save(imagePath);
      images[j].second.save(segmentationPath);
      ts << imagePath << " " << segmentationPath;
      if((j!=images.size()-1) || (i!=infos.size()-1))
        ts << "\n";
    }
  }
  listFile.close();
}

void ImageLibrary::saveSegmentationImagesJaccardIndex(QDir &dest, const QString &text, const QList<ImageInfo> &infos)
{
  QDir subDir(dest.absoluteFilePath(text));
  QDir().mkpath(subDir.path());
  foreach(ImageInfo info, infos)
  {
    QString imagePath = subDir.absoluteFilePath(info.getFilename());
    info.getImageSegmentation().save(imagePath);
  }
}

void ImageLibrary::saveImagePatches(QPixmap &image, QPixmap &segmentation, int size, QTextStream &completeStream, QTextStream &dataStream, QTextStream &labelStream, QDir &imagePath, size_t &trainCtr)
{
  for(int x=0;x<image.width();x+=size)
  {
    for(int y=0;y<image.height();y+=size)
    {
      QRect rect(x,y,size,size);
      QString filepathImage = imagePath.absoluteFilePath("image_"+QString::number(trainCtr)+".png");
      QString filepathLabel = imagePath.absoluteFilePath("label_"+QString::number(trainCtr)+".png");
      image.copy(rect).save(filepathImage);
      segmentation.copy(rect).save(filepathLabel);
      trainCtr++;
      completeStream << filepathImage << " " << filepathLabel << "\n";
      dataStream << filepathImage << "\n";
      labelStream << filepathLabel << "\n";
    }
  }
}

QString ImageLibrary::getPointCount()
{
  QString result;
  foreach(const ImageInfo& info, images)
  {
    if(info.isInTestSet())
      result += "{\"" + info.getFilename() + "\", " + QString::number(info.getPointSections().size()) + "},\n";
  }
  return result;
}

ImageLibrary::DirUpdateResult ImageLibrary::updateFiledirectory(const QDir &dir)
{
  bool allUpdated = true;
  bool noneUpdated = true;
  for(int i=0;i<images.size();i++)
  {
    ImageInfo& info = images[i];
    bool ret = info.updateFiledirectory(dir);
    allUpdated = allUpdated && ret;
    noneUpdated = allUpdated && !ret;
  }
  if(allUpdated && noneUpdated)
    return UPDATE_NOIMAGES;
  else if(allUpdated && !noneUpdated)
    return UPDATE_ALL;
  else if(!allUpdated && noneUpdated)
    return UPDATE_NONE;
  else
    return UPDATE_SOME;
}

bool ImageLibrary::allImagesExist()
{
  foreach(const ImageInfo& info, images)
  {
    if(!info.imageExists())
    {
      return false;
    }
  }
  return true;
}

void ImageLibrary::generateSegmentationData(int size, const QDir& dir)
{
  QDir trainPath(dir.absoluteFilePath("train"));
  QDir testPath(dir.absoluteFilePath("test"));
  QDir().mkpath(trainPath.absolutePath());
  QDir().mkpath(testPath.absolutePath());
  QFile trainCompleteFile(dir.absoluteFilePath("trainComplete.txt"));
  QFile trainDataFile(    dir.absoluteFilePath("trainData.txt"));
  QFile trainLabelFile(   dir.absoluteFilePath("trainLabel.txt"));
  QFile testCompleteFile( dir.absoluteFilePath("testComplete.txt"));
  QFile testDataFile(    dir.absoluteFilePath("testData.txt"));
  QFile testLabelFile(   dir.absoluteFilePath("testLabel.txt"));
  size_t trainCtr = 0;
  size_t testCtr  = 0;
  if(trainCompleteFile.open(QIODevice::WriteOnly) &&
     trainDataFile.open(QIODevice::WriteOnly) &&
     trainLabelFile.open(QIODevice::WriteOnly) &&
     testDataFile.open(QIODevice::WriteOnly) &&
     testCompleteFile.open(QIODevice::WriteOnly) &&
     testLabelFile.open(QIODevice::WriteOnly))
  {
    QTextStream trainCompleteStream(&trainCompleteFile);
    QTextStream trainDataStream(&trainDataFile);
    QTextStream trainLabelStream(&trainLabelFile);
    QTextStream testCompleteStream(&testCompleteFile);
    QTextStream testDataStream(&testDataFile);
    QTextStream testLabelStream(&testLabelFile);
    foreach(ImageInfo info, images)
    {
      QPixmap segmentation = info.getImageSegmentation();
      QPixmap image        = QPixmap(info.getFilepath());
      if(info.isInTestSet())
      {
        saveImagePatches(image,
                         segmentation,
                         size,
                         testCompleteStream,
                         testDataStream,
                         testLabelStream,
                         testPath,
                         testCtr);
      }
      else if(info.isInTrainingSet())
      {
        saveImagePatches(image,
                         segmentation,
                         size,
                         trainCompleteStream,
                         trainDataStream,
                         trainLabelStream,
                         trainPath,
                         trainCtr);
      }
    }
  }
  if(trainCompleteFile.isOpen())
    trainCompleteFile.close();
  if(trainDataFile.isOpen())
    trainDataFile.close();
  if(trainLabelFile.isOpen())
    trainLabelFile.close();
  if(testDataFile.isOpen())
    testDataFile.close();
  if(testCompleteFile.isOpen())
    testCompleteFile.close();
  if(testLabelFile.isOpen())
    testLabelFile.close();
}
