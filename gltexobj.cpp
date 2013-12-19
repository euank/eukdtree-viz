#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <QImage>
#include <QTimer>
#include <QEvent>
#include <QString>
#include <QFileDialog>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>

#include "tree.h"
#include "gltexobj.h"

#define PI 3.14159265

const int redrawWait = 50;
//const int redrawWait = 0;

GLTexobj::GLTexobj( QWidget* parent ) :
  QGLWidget( parent ),
  img(64,64,QImage::Format_RGB32)
{
  treeOff = false;
  knodes = 3;
  setMouseTracking(true);
  // create a GLTexobj widget
  timer = new QTimer( this );
  connect( timer, SIGNAL(timeout()), SLOT(update()) );
  timer->setInterval(redrawWait);
  timer->setSingleShot(FALSE);
  timer->start();
}


GLTexobj::~GLTexobj()
{
  // release allocated resources
  makeCurrent();
}


void GLTexobj::initializeGL()
{
  glClearColor( 0.0, 0.0, 0.0, 0.0 );
}

void GLTexobj::resizeGL( int w, int h )
{
  // set up the OpenGL view port
  glViewport(0,0,w,h);

  // set up the view transformation
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0,8.0/6.0,0.1,100.0);

  // set up the model transformation
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}


void GLTexobj::paintGL()
{
  glColor4f(.2,1,.2,0);
  // draw to the back buffer
  glDrawBuffer(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT);

  //model/view transformation
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho (0, width(), height(), 0, 0, 1);
  glDisable(GL_DEPTH_TEST);	
  glMatrixMode(GL_MODELVIEW);

  glColor4f(.2,1,.2,0);
  if(!points.empty())
  {
    for(int i=0; i<(int)points.size();i++)
    {

      glPointSize(4.0);
      glBegin(GL_POINTS);
      glVertex2f((*points[i])[0], (*points[i])[1]);
      glEnd();
      if(!treeOff)
      {
        std::vector<point_t> lines = pointTree.getLines();

        for(int i =0; i < lines.size(); i+=2)
        {
          if(lines[i][0] == lines[i+1][0])
            glColor4f(1,0,0,0);
          else
            glColor4f(0,1,0,0);
          glBegin(GL_LINES);
          glVertex2f(lines[i][0],lines[i][1]);
          glVertex2f(lines[i+1][0],lines[i+1][1]);
          glEnd();
        }
      }
    }
    if(rangeActive)
    {
      glColor4f(1,1,0,0);
      glBegin(GL_LINES);
      glVertex2f(rangeStart[0], rangeStart[1]);
      glVertex2f(mouseCoords[0], rangeStart[1]);

      glVertex2f(mouseCoords[0], rangeStart[1]);
      glVertex2f(mouseCoords[0], mouseCoords[1]);

      glVertex2f(mouseCoords[0], mouseCoords[1]);
      glVertex2f(rangeStart[0], mouseCoords[1]);

      glVertex2f(rangeStart[0], mouseCoords[1]);
      glVertex2f(rangeStart[0], rangeStart[1]);
      glEnd();

      glColor4f(1,0,1,0);
      rangePoints.clear();

      point_t miny(0,0);
      miny[0] = rangeStart[0] < mouseCoords[0] ? rangeStart[0] : mouseCoords[0];
      point_t maxy(0,0);
      maxy[0] = rangeStart[0] > mouseCoords[0] ? rangeStart[0] : mouseCoords[0];
      miny[1] = rangeStart[1] < mouseCoords[1] ? rangeStart[1] : mouseCoords[1];
      maxy[1] = rangeStart[1] > mouseCoords[1] ? rangeStart[1] : mouseCoords[1];


      pointTree.range(miny,	maxy,	rangePoints);
      for(int i=0;i<rangePoints.size();i++)
      {
        glBegin(GL_POINTS);
        glVertex2f((*rangePoints[i])[0], (*rangePoints[i])[1]);
        glEnd();
      }
    }
    if(nnActive)
    {
      query = mouseCoords;
      pointTree.nn(query, nearest, radius);
      glColor4f(1,.6,.2,0);
      glBegin(GL_POINTS);
      glVertex2f((*nearest)[0], (*nearest)[1]);
      glEnd();

      glColor4f(1,.8,0,0);
      glBegin(GL_LINE_LOOP);
      for(int i=0;i<=360;i++)
      {
        glVertex2f((mouseCoords[0] + sin(i*PI/180.0) * radius), 
            (mouseCoords[1] + cos(i*PI/180.0) * radius));
      }
      glEnd();
    }
    if(knnActive)
    {
      knnPoints.clear();
      query = mouseCoords;
      pointTree.knn(query, knnPoints, radius,knodes);
      glColor4f(1,.6,.2,0);
      glBegin(GL_POINTS);
      for(int i=0;i<knnPoints.size();i++)
        glVertex2f((*knnPoints[i])[0], (*knnPoints[i])[1]);
      glEnd();

      glColor4f(1,.8,0,0);
      glBegin(GL_LINE_LOOP);
      for(int i=0;i<=360;i++)
      {
        glVertex2f((mouseCoords[0] + sin(i*PI/180.0) * radius), (mouseCoords[1] + cos(i*PI/180.0) * radius));
      }
      glEnd();

    }
  }

  glColor4f(1,1,1,0);
  glPointSize(5.0);
  glBegin(GL_POINTS);
  glVertex2f(mouseCoords[0], mouseCoords[1]);
  glEnd();
}

void GLTexobj::mousePressEvent(QMouseEvent* e)
{
  shiftPressed = (QApplication::keyboardModifiers() == Qt::ShiftModifier);
  controlPressed = (QApplication::keyboardModifiers() == Qt::ControlModifier);
  if(e->button() == Qt::LeftButton && shiftPressed)
  {
    // NN time
    nnActive = true;
  }
  else if(e->button() == Qt::LeftButton && controlPressed)
  {
    knnActive = true;
  }
  else if(e->button() == Qt::LeftButton)
  {
    points.push_back(new point_t(e->x(),e->y()));
    pointTree.clear();
    pointTree.insert(points, point_t(0,0),point_t(width(),height()));
  }
  else if(e->button() == Qt::RightButton)
  {
    if(!rangeActive)
    {
      rangeStart = point_t(e->x(),e->y());
      rangeActive = true;
    }
  }

  updateGL();
}
void GLTexobj::mouseReleaseEvent(QMouseEvent* e)
{
  nnActive = false;
  knnActive = false;
  rangeActive = false;
}
void GLTexobj::mouseMoveEvent(QMouseEvent* e)
{
  mouseCoords = point_t(e->x(), e->y());
  e->accept();
  updateGL();
}


void GLTexobj::update()
{
  updateGL();
}

void GLTexobj::imgSaveAs()
{
  glReadBuffer(GL_BACK);

  QImage img = QGLWidget::grabFrameBuffer(true);

  QString qfilename = QFileDialog::getSaveFileName(this,
      tr("Save Image As..."),
      "./",
      tr("Images (*.png)"));

  // get QImage to write itself
  if(!qfilename.isEmpty()) img.save(qfilename,"PNG");
}

void GLTexobj::fileOpen()
{
  QString qfilename = QFileDialog::getOpenFileName(this,
      tr("Open File"),
      "./",
      tr("Point Files (*.pts)"));

  std::ifstream photons_ifs;    // input file

  if(!qfilename.isEmpty()) {
    //todo handle error
  }

  updateGL();
}

void GLTexobj::clearTree()
{
  nnActive = false;
  knnActive = false;
  rangeActive = false;
  points.clear();
  updateGL();
}

void GLTexobj::increaseK()
{ knodes++; if(knodes < 1) knodes = 1; }
void GLTexobj::decreaseK()
{ knodes--;if(knodes < 1) knodes = 1; }

