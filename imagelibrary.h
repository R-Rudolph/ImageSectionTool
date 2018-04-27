#ifndef IMAGELIBRARY_H
#define IMAGELIBRARY_H

#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QStringList>
#include "imageinfo.h"
#include "labeledimageset.h"

class ImageLibrary
{
  QList<ImageInfo> images;
  bool saveFileList(const QList<ImageInfo*>& list, const QString& filename);
public:
  enum LoadResult
  {
    LOAD_OPENFAIL,
    LOAD_PARSEFAIL,
    LOAD_IMAGEMISSING,
    LOAD_SUCCESS
  };
  enum DirUpdateResult
  {
    UPDATE_NONE,
    UPDATE_SOME,
    UPDATE_ALL,
    UPDATE_NOIMAGES
  };
  ImageLibrary();
  LoadResult load(const QString& libraryFilepath);
  bool save(const QString& libraryFilepath);
  void addImage(const QString& filename);
  void addImages(const QStringList& filenames);
  QStringList getFileNames();
  ImageInfo* infoByFilename(const QString& filename);
  QList<const ImageSection *> getSections(const QString& filename);
  const QList<ImageInfo>& getInfos();

  void removeImageSection(const QString& filename, const ImageSection& section);
  void addImageSection(const QString& filename, const ImageSection& section);

  void removeImageRadSection(const QString& filename, const ImageRadialSection& section);
  void addImageRadSection(const QString& filename, const ImageRadialSection& section);

  void removeImagePointSection(const QString& filename, const ImagePointSection& section);
  void addImagePointSection(const QString& filename, const ImagePointSection& section);

  void generateClassificationData(int width, int height, const QDir& dir);
  void saveSegmentationImages(QDir& dest);
  void saveSegmentationImagesSegNet(QDir& dest);
  void saveSegmentationImagesJaccardIndex(QDir& dest);
  void writeList(const QDir &dest,const QString& filename, const QList<ImageInfo>& infos);
  void saveSegmentationImagesSetNet(QDir &dest, const QString& text, const QList<ImageInfo>& infos);
  void saveSegmentationImagesJaccardIndex(QDir &dest, const QString& text, const QList<ImageInfo>& infos);

  void generateSegmentationData(int size, const QDir& dir);
  void saveImagePatches(QPixmap& image,
                        QPixmap& segmentation,
                        int size,
                        QTextStream& completeStream,
                        QTextStream& dataStream,
                        QTextStream& labelStream,
                        QDir& imagePath,
                        size_t& trainCtr);
  QString getPointCount();
  DirUpdateResult updateFiledirectory(const QDir& dir);
  bool allImagesExist();
};

#endif // IMAGELIBRARY_H
