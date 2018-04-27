#include "imagesectiontoolwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  ImageSectionToolWindow w;
  w.show();

  return a.exec();
}
