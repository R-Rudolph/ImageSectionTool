#ifndef TESTDATASELECTIONWINDOW_H
#define TESTDATASELECTIONWINDOW_H

#include <QWidget>

class QTreeWidget;
class ImageLibrary;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;

class TestDataSelectionWindow : public QWidget
{
  Q_OBJECT
  QHBoxLayout* buttonLayout;
  QVBoxLayout* mainLayout;
  QTreeWidget* list;
  QPushButton* randomizeButton;
  QPushButton* cancelButton;
  QPushButton* okButton;

  ImageLibrary* library;
  void saveChanges();
public:
  explicit TestDataSelectionWindow(QWidget *parent = nullptr);
  void setLibrary(ImageLibrary* library);
  void updateList();
signals:
  void changed();
public slots:
  void ok();
  void randomize();
};

#endif // TESTDATASELECTIONWINDOW_H
