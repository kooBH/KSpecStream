
#include <QApplication>
#include <QCoreApplication>

#include "KSpecStream.h"

int main(int argc, char* argv[]) {

  QApplication app(argc, argv);
  QCoreApplication::addLibraryPath("./lib");

  KSpecStream widget;

  widget.show();


  return app.exec();
}