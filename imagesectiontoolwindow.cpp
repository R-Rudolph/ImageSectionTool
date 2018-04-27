#include "imagesectiontoolwindow.h"

#include <QInputDialog>
#include <QClipboard>

void ImageSectionToolWindow::updateWindowTitle()
{
  QString title = QApplication::applicationName();
  if(!fileName.isEmpty())
  {
    QString f = fileName.split('/').last();
    if(pendingSave)
      f.append(" *");
    f = "["+f+"] ";
    title = f + title;
  }

  setWindowTitle(title);
}

void ImageSectionToolWindow::load()
{
  if(imageLib.load(fileName))
  {
    if(imageLib.allImagesExist())
    {
      updateFileList();
    }
    else
    {
      bool repeat = true;
      while(repeat)
      {
        if(QMessageBox::question(this,"Load Error","Some images listed in the data file could not be found.\n"
                                                   "Select new image directory?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
        {
          QString dirString = QFileDialog::getExistingDirectory(nullptr,"Select Image Directory");
          QDir dir(dirString);
          ImageLibrary::DirUpdateResult dirUpdate = imageLib.updateFiledirectory(dir);
          if((dirUpdate == ImageLibrary::UPDATE_ALL) || (dirUpdate == ImageLibrary::UPDATE_SOME))
          {
            setSaveStatus(true);
          }
          if(imageLib.allImagesExist())
          {
            repeat = false;
            updateFileList();
          }
          else
          {
            repeat = true;
          }
        }
        else
        {
          repeat = false;
          updateFileList();
        }
      }
    }
  }
  else
  {
    QMessageBox::warning(this,"Load Error","Could not load data file:\n"+fileName,QMessageBox::Ok,QMessageBox::Ok);
  }
}

bool ImageSectionToolWindow::save()
{
  if(imageLib.save(fileName))
  {
    setSaveStatus(false);
    pendingSave = false;
    updateWindowTitle();
    return true;
  }
  else
  {
    QMessageBox::warning(this,"Save Error","Could not save file:\n"+fileName,QMessageBox::Ok,QMessageBox::Ok);
    return false;
  }
}

void ImageSectionToolWindow::updateFileList()
{
  //imageSelection->setFiles(imageLib.getFileNames());
  imageSelection->setInfos(imageLib.getInfos());
}

void ImageSectionToolWindow::setSaveStatus(bool savePossible)
{
  pendingSave = savePossible;
  updateWindowTitle();
  saveAction->setDisabled(!savePossible);
}

void ImageSectionToolWindow::setTool(ImageTool tool)
{
  for(int i=0;i<toolMenu->actions().size();i++)
  {
    toolMenu->actions()[i]->setIcon(QIcon());
  }
  if(toolActionMap.contains(tool))
  {
    toolActionMap[tool]->setIcon(QIcon::fromTheme("checkmark"));
  }
  imageScene->setTool(tool);
}

ImageSectionToolWindow::ImageSectionToolWindow(QWidget *parent)
  : QMainWindow(parent), settings(this)
{
  fileName = settings.value("lastfile","").toString();
  QMenu* fileMenu = this->menuBar()->addMenu("File");
  fileMenu->addAction("Load",this,&ImageSectionToolWindow::loadFile);
  saveAction = fileMenu->addAction("Save",this,&ImageSectionToolWindow::saveFile,QKeySequence(Qt::CTRL+Qt::Key_S));
  fileMenu->addAction("Save As...",this,&ImageSectionToolWindow::saveAsFile);
  fileMenu->addSeparator();
  fileMenu->addAction("Exit",this,&ImageSectionToolWindow::close);
  toolMenu = this->menuBar()->addMenu("Annotation");
  toolActionMap.insert(ImageTool::Section,toolMenu->addAction("Section",this,&ImageSectionToolWindow::toolActionSection));
  //toolActionMap.insert(ImageTool::RadialSection,toolMenu->addAction("Radial Section",this,&ImageSectionToolWindow::toolActionRadSection));
  toolActionMap.insert(ImageTool::Point,toolMenu->addAction("Point",this,&ImageSectionToolWindow::toolActionPoint));
  QMenu* imageMenu = this->menuBar()->addMenu("Image");
  imageMenu->addAction("Add Images...",this,&ImageSectionToolWindow::addImages);
  imageMenu->addAction("Select Test Data",this,&ImageSectionToolWindow::selectTestData);
  QMenu* extractMenu = this->menuBar()->addMenu("Extract Data");
  extractMenu->addAction("Classification Data",this,&ImageSectionToolWindow::generateClassificationData);
  extractMenu->addAction("Segmentation Data",this,&ImageSectionToolWindow::generateSegmentationData);
  extractMenu->addAction("Full Segmentation Data",this,&ImageSectionToolWindow::saveFullSegmentationImages);
  //extractMenu->addAction("Classification Data",this,&ImageSectionToolWindow::saveSegmentationImages);
  //extractMenu->addAction("Segmentation Data",this,&ImageSectionToolWindow::saveSegmentationImagesSegNet);
  setSaveStatus(false);

  splitter = new QSplitter(Qt::Horizontal,this);
  imageSelection = new ImageSelectionWidget(this);
  imageView = new ImageView(this);
  imageScene = new ImageScene(this);
  imageView->setScene(imageScene);
  this->setCentralWidget(splitter);
  splitter->addWidget(imageSelection);
  splitter->addWidget(imageView);
  splitter->setStretchFactor(0,1);
  splitter->setStretchFactor(1,3);

  connect(imageSelection,&ImageSelectionWidget::fileSelected,this,&ImageSectionToolWindow::imageFileSelected);
  connect(imageScene,&ImageScene::sectionGenerated,this,&ImageSectionToolWindow::addImageRegion);
  connect(imageScene,&ImageScene::sectionRemove,this,&ImageSectionToolWindow::removeImageRegion);

  connect(imageScene,&ImageScene::radSectionGenerated,this,&ImageSectionToolWindow::addImageRadRegion);
  connect(imageScene,&ImageScene::radSectionRemove,this,&ImageSectionToolWindow::removeImageRadRegion);

  connect(imageScene,&ImageScene::pointSectionGenerated,this,&ImageSectionToolWindow::addImagePointRegion);
  connect(imageScene,&ImageScene::pointSectionRemove,this,&ImageSectionToolWindow::removeImagePointRegion);

  testDataWindow = new TestDataSelectionWindow(this);
  testDataWindow->setLibrary(&imageLib);

  connect(testDataWindow,&TestDataSelectionWindow::changed,this,&ImageSectionToolWindow::changed);

  updateWindowTitle();
  load();
  setTool(ImageTool::Section);
}

ImageSectionToolWindow::~ImageSectionToolWindow()
{

}

void ImageSectionToolWindow::setFileName(const QString &value)
{
  fileName = value;
  updateWindowTitle();
  settings.setValue("lastfile",value);
}

void ImageSectionToolWindow::closeEvent(QCloseEvent *event)
{
  if(pendingSave)
  {
    QMessageBox::StandardButton button = QMessageBox::question(this,
                                                               "Save Changes",
                                                               "Do you want to save the changes before quitting?",
                                                               QMessageBox::No | QMessageBox::Cancel | QMessageBox::Save,
                                                               QMessageBox::Save);
    if(button==QMessageBox::Cancel)
      event->ignore();
    else if(button==QMessageBox::Save)
    {
      if(save())
        event->accept();
      else
        event->ignore();
    }
    else
    {
      event->accept();
    }
  }
  else
  {
    event->accept();
  }
}

void ImageSectionToolWindow::loadFile()
{
  QString fileName = QFileDialog::getOpenFileName(this,"Select Data File",this->fileName);
  if(!fileName.isEmpty())
  {
    setFileName(fileName);
    load();
  }
}

void ImageSectionToolWindow::saveFile()
{
  if(fileName.isEmpty())
    saveAsFile();
  else
  {
    save();
  }
}

void ImageSectionToolWindow::saveAsFile()
{
  QString fileName = QFileDialog::getSaveFileName(this,"Save File",this->fileName);
  if(!fileName.isEmpty())
  {
    setFileName(fileName);
    save();
  }
}

void ImageSectionToolWindow::addImages()
{
  QStringList filenames = QFileDialog::getOpenFileNames(this,"Add Images");
  imageLib.addImages(filenames);
  setSaveStatus(true);
  updateFileList();
}

void ImageSectionToolWindow::generateClassificationData()
{
  bool ok;
  int size = QInputDialog::getInt(this,"Segmentation Image Size","How big should the image patches be?",227,1,1000000,1,&ok);
  if(!ok)
    return;
  QString dirpath = QFileDialog::getExistingDirectory(this,"Select Target Folder");
  QDir dir(dirpath);
  if(dir.exists())
  {
    imageLib.generateClassificationData(size,size,dir);
  }
}

void ImageSectionToolWindow::generateSegmentationData()
{
  bool ok;
  int size = QInputDialog::getInt(this,"Segmentation Image Size","How big should the image patches be?",608,1,1000000,1,&ok);
  if(!ok)
    return;
  QString dirPath = QFileDialog::getExistingDirectory(this,"Select Target Folder");
  QDir dir(dirPath);
  if(dir.exists() && !dirPath.isEmpty())
  {
    imageLib.generateSegmentationData(size,dir);
  }
}

void ImageSectionToolWindow::imageFileSelected(const QString &file)
{
  imageFilename = file;
  imageScene->setImageInfo(imageLib.infoByFilename(file));
}

void ImageSectionToolWindow::removeImageRegion(const ImageSection &section)
{
  imageLib.removeImageSection(imageFilename,section);
  setSaveStatus(true);
}

void ImageSectionToolWindow::addImageRegion(const ImageSection &section)
{
  imageLib.addImageSection(imageFilename,section);
  setSaveStatus(true);
}

void ImageSectionToolWindow::removeImageRadRegion(const ImageRadialSection &section)
{
  imageLib.removeImageRadSection(imageFilename,section);
  setSaveStatus(true);
}

void ImageSectionToolWindow::addImageRadRegion(const ImageRadialSection &section)
{
  imageLib.addImageRadSection(imageFilename,section);
  setSaveStatus(true);
}

void ImageSectionToolWindow::removeImagePointRegion(const ImagePointSection &section)
{
  imageLib.removeImagePointSection(imageFilename,section);
  setSaveStatus(true);
}

void ImageSectionToolWindow::addImagePointRegion(const ImagePointSection &section)
{
  imageLib.addImagePointSection(imageFilename,section);
  setSaveStatus(true);
}

void ImageSectionToolWindow::saveFullSegmentationImages()
{
  QString string = QFileDialog::getExistingDirectory(nullptr,"Select Save Dir");
  if(string.isEmpty())
    return;
  QDir dir(string);
  imageLib.saveSegmentationImages(dir);
}
/*
void ImageSectionToolWindow::saveSegmentationImages()
{
  QString dirpath = QFileDialog::getExistingDirectory(this,"Choose Target Directory");
  QDir dir(dirpath);
  if(dir.exists() && !dirpath.isEmpty())
  {
    imageLib.saveSegmentationImages(dir);
  }
}

void ImageSectionToolWindow::saveSegmentationImagesSegNet()
{
  QString dirpath = QFileDialog::getExistingDirectory(this,"Choose Target Directory");
  QDir dir(dirpath);
  if(dir.exists() && !dirpath.isEmpty())
  {
    imageLib.saveSegmentationImagesSegNet(dir);
  }
}
*/

void ImageSectionToolWindow::toolActionSection()
{
  setTool(ImageTool::Section);
}

void ImageSectionToolWindow::toolActionRadSection()
{
  setTool(ImageTool::RadialSection);
}

void ImageSectionToolWindow::toolActionPoint()
{
  setTool(ImageTool::Point);
}

void ImageSectionToolWindow::selectTestData()
{
  testDataWindow->updateList();
  testDataWindow->show();
  testDataWindow->raise();
}

void ImageSectionToolWindow::changed()
{
  updateFileList();
  setSaveStatus(true);
}
