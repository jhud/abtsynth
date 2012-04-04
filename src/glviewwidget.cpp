#include "glviewwidget.h"
#include "skeleton.h"
#include "bone.h"

#include <QTimer>
#include <GL/glu.h>
#include <qevent.h>

GlViewWidget::GlViewWidget(QWidget *parent) :
    QGLWidget(parent)
{
    mSkeleton = new Skeleton(this);
    mSkeleton->resolve();

    QTimer * timer;
    timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
    timer->start(100);
}

void GlViewWidget::initializeGL()
{
    glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );
}

void GlViewWidget::resizeGL( int w, int h )
{
    if ( h == 0 )
        h = 1;

    // Set the viewport to window dimensions
    glViewport( 0, 0, w, h );

    // reset the coordinate system
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    mAspectRatio = double( w ) / double( h );

    gluPerspective(90.0, mAspectRatio, 0.5, 500);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void GlViewWidget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT );
    glColor3f( 1.0f, 0.0f, 0.0f );

    glPushMatrix();
    glTranslatef(0,0,-5);
    mSkeleton->render();
    glPopMatrix();
}

void GlViewWidget::mouseMoveEvent(QMouseEvent * me)
{
    me->accept();

    mSkeleton->mRoot->mPos[0].setX((float)me->x()/50.0f);

    mSkeleton->mRoot->mPos[0].setY((float)me->y()/-50.0f);

    mSkeleton->resolve();
    updateGL();
}

void GlViewWidget::tick()
{
//    mSkeleton->applyForce(-0.05);

//    QList<Bone*> bl;
//    mSkeleton->separate(mSkeleton->mRoot, &bl);

    mSkeleton->resolve();

    updateGL();
}

