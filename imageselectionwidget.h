#ifndef IMAGESELECTIONWIDGET_H
#define IMAGESELECTIONWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMap>
#include <QString>
#include <QStackedLayout>

#include "imageinfo.h"

class ImageSelectionWidget : public QWidget
{
  Q_OBJECT
  //layout and widgets
  QStackedLayout* layout;
  QTreeWidget* fileList;
  //member variables
  bool editing;
  QString currentFilename;
  //member functions
  QTreeWidgetItem* itemFromInfo(const ImageInfo& info);
public:
  explicit ImageSelectionWidget(QWidget *parent = 0);
signals:
  void fileSelected(const QString& filename);
public slots:
  void setInfos(const QList<ImageInfo>& files);
private slots:
  void fileSelectedSlot(QTreeWidgetItem* current, QTreeWidgetItem* previous);
};

#endif // IMAGESELECTIONWIDGET_H
