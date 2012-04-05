#include "glviewwidget.h"
#include "skeleton.h"
#include "bone.h"

#include <math.h>
#include <QTimer>
#include <GL/glu.h>
#include <qevent.h>
#include <QMessageBox>

GLUquadricObj * qo;

GlViewWidget::GlViewWidget(QWidget *parent) :
    QGLWidget(parent)
{
   qo = gluNewQuadric();

    mSkeleton = new Skeleton(this);
    if (!mSkeleton->load("../charabesque/data/guy.xml")) {
        new QMessageBox(QMessageBox::Critical, "Error", "Could not load skeleton.");
    }
    else {
    mSkeleton->resolve();

    QTimer * timer;
    timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
    timer->start(100);
    }
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
    glColor3f( 1.0f, 1.0f, 1.0f );

    glPushMatrix();
    glTranslatef(0,0,-2);
    static float xx; xx+= 0.01;
    glRotatef(sin(xx)*30, 0, 1, 0);
  //  glLineWidth(2.0);
    mSkeleton->render();

    {
        glColor3f( 1.0f, 1.0f, 1.0f );

        glPushMatrix();
        glTranslated(mSkeleton->mRoot->start().x(), mSkeleton->mRoot->start().y(), mSkeleton->mRoot->start().z());
        gluSphere(qo, 0.15, 8, 8);
        glPopMatrix();
    }

    glPopMatrix();
}

void GlViewWidget::mouseMoveEvent(QMouseEvent * me)
{
    me->accept();

    if (!mSkeleton || !mSkeleton->mRoot) {
        return;
    }

    mSkeleton->mRoot->setStart((float)me->x()/50.0f, (float)me->y()/-50.0f, 0);

    mSkeleton->resolve();
    updateGL();
}

void GlViewWidget::tick()
{
//    mSkeleton->applyForce(-0.05);

//    QList<Bone*> bl;
//    mSkeleton->separate(mSkeleton->mRoot, &bl);

//    mSkeleton->resolve();

    updateGL();
}

