#include <iostream>

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QVBoxLayout>

#include "glwinobj.h"
#include "gltexobj.h"

GLObjectWindow::GLObjectWindow( QWidget* parent, Qt::WindowFlags f ) :
  QWidget( parent, f )
{
  // Create an OpenGL widget: (doubleBuffer | rgba | depth) set globally
  GLTexobj* c = new GLTexobj(this);
  std::cout << "doubleBuffer: "	<< c->format().doubleBuffer()	<< " "
    << "rgba: "		<< c->format().rgba()		<< " "
    << "depth: "		<< c->format().depth()		<< " "
    << std::endl;
  //  c->setMouseTracking(true);

  // create the file menu
  QMenu *file = new QMenu("File",this);
  file->addAction("Save Image...",c,SLOT(imgSaveAs()),Qt::CTRL+Qt::Key_S);
  // doesn't seem to be needed in Qt 4.6
  file->addAction("Quit",QApplication::instance(),SLOT(quit()),
      Qt::CTRL+Qt::Key_Q);

  QMenu *tree = new QMenu("Tree",this);
  tree->addAction("Clear Tree...",c,SLOT(clearTree()),Qt::CTRL+Qt::Key_C);
  tree->addAction("Increase Knodes +1",c,SLOT(increaseK()),Qt::CTRL+Qt::Key_P);
  tree->addAction("Decrease Knodes -1",c,SLOT(decreaseK()),Qt::CTRL+Qt::Key_Minus);

  // create a menu bar
  QMenuBar *m = new QMenuBar(this);
  m->addSeparator();
  m->addMenu(file);
  m->addMenu(tree);

  // top level layout (with 0,0 border)
  QVBoxLayout* vlayout = new QVBoxLayout(this);

  // no border, no margin
  vlayout->setSpacing(0);
  vlayout->setMargin(0);

  // add menu bar and GL window
  vlayout->setMenuBar(m);
  vlayout->addWidget(c);
}
