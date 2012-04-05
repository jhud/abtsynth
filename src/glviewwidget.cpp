#include "glviewwidget.h"
#include "skeleton.h"
#include "bone.h"

#include <math.h>
#include <QTimer>
#include <GL/glu.h>
#include <qevent.h>
#include <QMessageBox>

GLUquadricObj * qo;
QVector3D pick;

GlViewWidget::GlViewWidget(QWidget *parent) :
    QGLWidget(parent)
{
   qo = gluNewQuadric();

   setFocus();

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
    glEnable(GL_DEPTH_TEST);
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

    gluPerspective(90.0, mAspectRatio, 0.5, 100);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslated(0,0,-2);
}

void GlViewWidget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f( 1.0f, 1.0f, 1.0f );

    glPushMatrix();
//    glTranslatef(0,0,-2);
    static float xx; xx+= 0.01;
 //   glRotatef(sin(xx)*30, 0, 1, 0);
    glLineWidth(2.0);
    mSkeleton->render();

    {
        glColor3f( 1.0f, 1.0f, 1.0f );

        glPushMatrix();
        glTranslated(mSkeleton->mRoot->start().x(), mSkeleton->mRoot->start().y(), mSkeleton->mRoot->start().z());
        gluSphere(qo, 0.15, 8, 8);
        glPopMatrix();
    }
    glPopMatrix();

    {
        glColor3f( 1.0f, 1.0f, 0.0f );

        glPushMatrix();
        glTranslated(pick.x(), pick.y(), pick.z());
        gluSphere(qo, 0.05, 8, 8);
        glPopMatrix();
    }

    if (mSkeleton->mSelected)
    {
        glColor3f( 0.0f, 1.0f, 0.0f );

        glPushMatrix();
        glTranslated(mSkeleton->mSelected->start().x(), mSkeleton->mSelected->start().y(), mSkeleton->mSelected->start().z());
        gluSphere(qo, 0.05, 8, 8);
        glPopMatrix();
    }
}

void GlViewWidget::mouseMoveEvent(QMouseEvent * me)
{
    me->accept();

    if (!mSkeleton || !mSkeleton->mRoot) {
        return;
    }

    QVector3D pick = screenToWorld(me->x(), me->y(), true);

    if (mSkeleton->mSelected) {
    mSkeleton->mSelected->setStart(pick.x(), pick.y(), 0);

    mSkeleton->resolve();
    updateGL();
    }
}

void GlViewWidget::mouseDoubleClickEvent(QMouseEvent *me)
{
    me->accept();

    if (!mSkeleton || !mSkeleton->mRoot) {
        return;
    }

    pick = screenToWorld(me->x(), me->y(), false);

    mSkeleton->selectBone(&pick);

    updateGL();
}

void GlViewWidget::keyPressEvent(QKeyEvent *ke)
{
    QVector3D & pos = mSkeleton->mSelected->start();

    switch(ke->key()) {
    case Qt::Key_Left:
        pos.setX(pos.x()-0.1);
        ke->accept();
        break;
    case Qt::Key_Right:
        pos.setX(pos.x()+0.1);
        ke->accept();
        break;
    case Qt::Key_Up:
        pos.setY(pos.y()+0.1);
        ke->accept();
        break;
    case Qt::Key_Down:
        pos.setY(pos.y()-0.1);
        ke->accept();
        break;
    }

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

QVector3D GlViewWidget::screenToWorld(int x, int y, bool ray)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

    gluUnProject( winX, winY, ray?0.85 : winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    return QVector3D(posX, posY, posZ);
}

