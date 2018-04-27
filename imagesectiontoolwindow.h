#ifndef IMAGESECTIONTOOLWINDOW_H
#define IMAGESECTIONTOOLWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QString>
#include <QMenuBar>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QStringList>
#include <QStackedLayout>
#include <QSplitter>
#include "imagelibrary.h"
#include "imageselectionwidget.h"
#include "imageview.h"
#include "imagescene.h"
#include "testdataselectionwindow.h"


class ImageSectionToolWindow : public QMainWindow
{
  Q_OBJECT
  //widgets and layouts
  QSplitter* splitter;
  ImageSelectionWidget* imageSelection;
  ImageView* imageView;
  ImageScene* imageScene;
  //member variables
  QSettings settings;
  QString fileName;
  QString imageFilename;
  bool pendingSave;
  ImageLibrary imageLib;
  //menu bar stuff
  QMap<ImageTool,QAction*> toolActionMap;
  QMenu* toolMenu;
  QAction* saveAction;
  //subwindows
  TestDataSelectionWindow* testDataWindow;

  void updateWindowTitle();
  void load();
  bool save();
  void updateFileList();
  void setSaveStatus(bool savePossible);
  void setTool(ImageTool tool);
public:
  ImageSectionToolWindow(QWidget *parent = 0);
  ~ImageSectionToolWindow();
  void setFileName(const QString &value);

protected:
  virtual void closeEvent(QCloseEvent * event);
private slots:
  void loadFile();
  void saveFile();
  void saveAsFile();
  void addImages();
  void generateClassificationData();
  void generateSegmentationData();
  void imageFileSelected(const QString& file);

  void removeImageRegion(const ImageSection& section);
  void addImageRegion(const ImageSection& section);

  void removeImageRadRegion(const ImageRadialSection& section);
  void addImageRadRegion(const ImageRadialSection& section);

  void removeImagePointRegion(const ImagePointSection& section);
  void addImagePointRegion(const ImagePointSection& section);

  void saveFullSegmentationImages();

  //void saveSegmentationImages();
  //void saveSegmentationImagesSegNet();
  void toolActionSection();
  void toolActionRadSection();
  void toolActionPoint();

  void selectTestData();
  void changed();
};

#endif // IMAGESECTIONTOOLWINDOW_H
