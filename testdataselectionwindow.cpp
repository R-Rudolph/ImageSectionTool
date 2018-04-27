#include "testdataselectionwindow.h"

#include "imagelibrary.h"
#include<QPushButton>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QTreeWidget>
#include<QTreeWidget>
#include<QInputDialog>
#include<QHeaderView>

void TestDataSelectionWindow::saveChanges()
{
  bool wasChanged = false;
  for(int i=0;i<list->topLevelItemCount();i++)
  {
    QTreeWidgetItem* item = list->topLevelItem(i);
    ImageInfo* info = library->infoByFilename(item->data(0,Qt::UserRole).toString());
    bool isTestData = (item->checkState(2) == Qt::Checked);
    if(info->isTestData() != isTestData)
    {
      info->setTestData(isTestData);
      wasChanged = true;
    }
    bool ignore = (item->checkState(3) == Qt::Checked);
    if(info->getIgnore() != ignore)
    {
      info->setIgnore(ignore);
      wasChanged = true;
    }
  }
  if(wasChanged)
    emit changed();
}

TestDataSelectionWindow::TestDataSelectionWindow(QWidget *parent) : QWidget(parent,Qt::Window)
{
  setWindowTitle("Test Data Selection");
  mainLayout = new QVBoxLayout(this);

  list = new QTreeWidget(this);
  mainLayout->addWidget(list);

  buttonLayout = new QHBoxLayout();
  mainLayout->addLayout(buttonLayout);

  okButton = new QPushButton(QIcon::fromTheme("dialog-ok"),"Ok",this);
  cancelButton = new QPushButton(QIcon::fromTheme("dialog-cancel"),"Cancel",this);
  randomizeButton = new QPushButton(QIcon::fromTheme("media-playlist-shuffle"),"Randomize",this);
  buttonLayout->addWidget(randomizeButton);
  buttonLayout->addWidget(okButton);
  buttonLayout->addWidget(cancelButton);

  list->setHeaderLabels({"n","Filename","Test Data","Ignore"});
  list->setRootIsDecorated(false);
  list->setSortingEnabled(true);
  list->sortByColumn(1,Qt::AscendingOrder);
  list->header()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
  list->header()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
  list->header()->setSectionResizeMode(3,QHeaderView::ResizeToContents);

  connect(okButton,&QPushButton::clicked,this,&TestDataSelectionWindow::ok);
  connect(cancelButton,&QPushButton::clicked,this,&TestDataSelectionWindow::close);
  connect(randomizeButton,&QPushButton::clicked,this,&TestDataSelectionWindow::randomize);
}

void TestDataSelectionWindow::setLibrary(ImageLibrary *library)
{
  this->library = library;
  updateList();
}

void TestDataSelectionWindow::updateList()
{
  list->clear();
  int ctr = 1;
  foreach(const QString& filename, library->getFileNames())
  {
    ImageInfo* info = library->infoByFilename(filename);
    QTreeWidgetItem* item = new QTreeWidgetItem({QString::number(ctr),info->getFilename()});
    item->setData(0,Qt::UserRole,filename);
    if(info->isTestData())
      item->setCheckState(2,Qt::Checked);
    else
      item->setCheckState(2,Qt::Unchecked);
    if(info->getIgnore())
      item->setCheckState(3,Qt::Checked);
    else
      item->setCheckState(3,Qt::Unchecked);
    list->addTopLevelItem(item);
    ctr++;
  }
}

void TestDataSelectionWindow::ok()
{
  saveChanges();
  close();
}

void TestDataSelectionWindow::randomize()
{
  int value = 0;
  for(int i=0;i<list->topLevelItemCount();i++)
  {
    if(list->topLevelItem(i)->checkState(2)==Qt::Checked)
      value++;
  }
  bool ok;
  int n = QInputDialog::getInt(this,"Randomize","How many test images should be selected?",value,0,list->topLevelItemCount(),1,&ok);
  if(ok)
  {
    QList<QTreeWidgetItem*> items;
    for(int i=0;i<list->topLevelItemCount();i++)
    {
      items.append(list->topLevelItem(i));
      list->topLevelItem(i)->setCheckState(2,Qt::Unchecked);
    }
    for(int i=0;i<n;i++)
    {
      int index = qrand()%items.size();
      items[index]->setCheckState(2,Qt::Checked);
      items.removeAt(index);
    }
  }
}
