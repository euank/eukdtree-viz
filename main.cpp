#include <QApplication>
#include <QGLFormat>

#include "glwinobj.h"

int main(int argc, char **argv)
{
  QApplication::setColorSpec(QApplication::CustomColor);
  QApplication app(argc,argv);

  if(!QGLFormat::hasOpenGL()) {
    qWarning( "This system has no OpenGL support. Exiting." );
    return -1;
  }

  // Create OpenGL format
  QGLFormat f;
  f.setDoubleBuffer(TRUE); f.setRgba(TRUE); f.setDepth(TRUE);
  QGLFormat::setDefaultFormat(f);

  GLObjectWindow* w = new GLObjectWindow;

  // set size...
  w->resize( 400, 350 );
  //w->resize( 1280, 1024 );
  w->show();
  // ... or go full screen
  //w->showFullScreen();

  int result = app.exec();
  delete w;
  return result;
}
