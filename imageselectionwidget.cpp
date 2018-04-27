#include "imageselectionwidget.h"

QTreeWidgetItem *ImageSelectionWidget::itemFromInfo(const ImageInfo &info)
{
  QTreeWidgetItem* item = new QTreeWidgetItem();
  item->setText(0,QString::number(fileList->topLevelItemCount()+1));
  item->setText(1,info.getFilename());
  if(info.isTestData())
  {
    item->setForeground(0,QBrush(QColor(192,64,0)));
    item->setForeground(1,QBrush(QColor(192,64,0)));
  }
  item->setData(0,Qt::UserRole,info.getFilepath());
  return item;
}

ImageSelectionWidget::ImageSelectionWidget(QWidget *parent) : QWidget(parent)
{
  editing = false;
  layout = new QStackedLayout(this);
  fileList = new QTreeWidget(this);
  layout->addWidget(fileList);
  fileList->setRootIsDecorated(false);
  fileList->setHeaderLabels(QStringList({"n","File"}));

  connect(fileList,&QTreeWidget::currentItemChanged,this,&ImageSelectionWidget::fileSelectedSlot);
}

void ImageSelectionWidget::setInfos(const QList<ImageInfo> &files)
{
  editing = true;
  while(fileList->topLevelItemCount()>0)
    delete fileList->topLevelItem(0);
  bool foundCurrent = false;
  for(int i=0;i<files.size();i++)
  {
    QTreeWidgetItem* item = itemFromInfo(files[i]);
    fileList->addTopLevelItem(item);
    if(item->data(0,Qt::UserRole).toString()==currentFilename)
    {
      foundCurrent = true;
      fileList->setCurrentItem(item);
    }
  }
  if(!foundCurrent)
    currentFilename = QString();
  emit fileSelected(currentFilename);
  editing = false;
}

void ImageSelectionWidget::fileSelectedSlot(QTreeWidgetItem *current, QTreeWidgetItem*)
{
  if(editing)
    return;
  currentFilename = current->data(0,Qt::UserRole).toString();
  emit fileSelected(currentFilename);
}
