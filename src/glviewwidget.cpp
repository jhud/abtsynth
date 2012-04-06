#include "glviewwidget.h"
#include "skeleton.h"
#include "bone.h"
#include "mainwindow.h"

#include <QDebug>
#include <math.h>
#include <QTimer>
#include <GL/glu.h>
#include <qevent.h>
#include <QMessageBox>

GLUquadricObj * qo;
QVector3D pick;
float pickedZ;

GlViewWidget::GlViewWidget(QWidget *parent) :
    QGLWidget(parent)
  , mSelectBoneEnds(false)
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
    qDebug() << "Viewport resized to " << w << "x" << h;
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

    updateGL();
}

void GlViewWidget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f( 1.0f, 1.0f, 1.0f );

    if (!mSkeleton) {
        return;
    }

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

        QVector3D * boneEnd = 0;
        if (mSelectBoneEnds == false) {
            boneEnd = &mSkeleton->mSelected->start();
        }
        else {
            boneEnd = &mSkeleton->mSelected->end();
        }

        glTranslated(boneEnd->x(), boneEnd->y(), boneEnd->z());
        gluSphere(qo, 0.02, 8, 8);
        glPopMatrix();
    }
}

void GlViewWidget::mouseMoveEvent(QMouseEvent * me)
{
    if (me->buttons() & Qt::LeftButton)
    {
        me->accept();

        if (!mSkeleton || !mSkeleton->mRoot) {
            return;
        }

        QVector3D pick = screenToWorldAlongPlane(me->x(), me->y(), pickedZ);

        if (mSkeleton->mSelected) {
            if (mSelectBoneEnds == false) {
                mSkeleton->mSelected->setStart(pick.x(), pick.y(), 0);
            }
            else {
                mSkeleton->mSelected->setEnd(pick.x(), pick.y(), 0);
            }

            mSkeleton->resolve();
            updateGL();
        }
    }
}

void GlViewWidget::mousePressEvent(QMouseEvent *me)
{
    if (me->button() == Qt::RightButton)
    {
        me->accept();

        if (!mSkeleton || !mSkeleton->mRoot) {
            return;
        }

        pick = screenToWorld(me->x(), me->y(), &pickedZ);

        mSkeleton->selectBone(&pick);

        if (mSkeleton->mSelected) {
            MainWindow::get().setStatusText(mSkeleton->mSelected->name());

            updateGL();
        }
    }
}

void GlViewWidget::keyPressEvent(QKeyEvent *ke)
{
    QVector3D & pos = (mSelectBoneEnds) ? mSkeleton->mSelected->end() : mSkeleton->mSelected->start();

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

QVector3D GlViewWidget::screenToWorld(int x, int y, float * normalizedZ)
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

    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    if(normalizedZ) {
        *normalizedZ = winZ;
    }

    return QVector3D(posX, posY, posZ);
}

QVector3D GlViewWidget::screenToWorldAlongPlane(int x, int y, float z)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY;
    GLdouble posX, posY, posZ;

    Q_ASSERT(qBound(0.0f, z, 1.0f) == z);

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;

    gluUnProject( winX, winY, z, modelview, projection, viewport, &posX, &posY, &posZ);

    return QVector3D(posX, posY, posZ);
}
