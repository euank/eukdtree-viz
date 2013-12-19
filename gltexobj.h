#ifndef GLTEXOBJ_H
#define GLTEXOBJ_H

#include <QGLWidget>
#include <QImage>
#include <QMouseEvent>
#include <vector>
#include "tree.h"
#include "point.h"

typedef enum { NVIDIA, ATI } vendor_t;

class GLTexobj : public QGLWidget
{
  Q_OBJECT

  public:

    GLTexobj( QWidget* parent );
    ~GLTexobj();

    public slots:
      void 		mousePressEvent(QMouseEvent* e);
    void		mouseReleaseEvent(QMouseEvent* e);
    void		mouseMoveEvent(QMouseEvent* e);
    void		update();
    void		imgSaveAs();
    void		fileOpen();
    void		clearTree();
    void		increaseK();
    void		decreaseK();

  protected:

    void		initializeGL();
    void		paintGL();
    void		resizeGL(int w, int h);

  private:
    GLuint		texName;
    QTimer*		timer;
    QImage		img;
    kdTree<point_t, point_t*, PointAxisCompare> pointTree;
    std::vector<point_t*> points;
    int				knodes;
    bool			rangeActive;
    bool			nnActive;
    bool			knnActive;
    bool 			shiftPressed;
    bool			controlPressed;
    bool			treeOff;
    point_t		mouseCoords;
    point_t		rangeStart;
    point_t		rangeEnd;
    std::vector<point_t*> rangePoints;
    point_t		*pointFromMouseCoords(int x, int y);
    point_t		query;
    point_t		*nearest;
    std::vector<point_t*>  knnPoints;
    double		radius;
};


#endif // GLTEXOBJ_H
