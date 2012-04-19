#include "glviewwidget.h"
#include "skeleton.h"
#include "bone.h"
#include "mainwindow.h"
#include "common.h"
#include "maths.h"

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
        timer->start(50);

        for (int i=0; i<100; i++) {
            addSpark();
        }
    }
}

void GlViewWidget::initializeGL()
{
    glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );
    glEnable(GL_DEPTH_TEST);
    glPointSize(0.5f);
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
    if (mRenderMode != Skeleton::RenderFinal) {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glColor3f( 1.0f, 1.0f, 1.0f );
    }

    if (!mSkeleton) {
        return;
    }

    glPushMatrix();
    static float xx; xx+= 0.01;
    //     glRotatef(sin(xx)*30, 0, 1, 0);
    //   glLineWidth(2.0);
    mSkeleton->render(mRenderMode);

    glPopMatrix();

    if (mSkeleton->mSelected && mRenderMode != Skeleton::RenderFinal)
    {
        glDisable(GL_DEPTH_TEST);
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
        glEnable(GL_DEPTH_TEST);
    }

    glDisable(GL_DEPTH_TEST);
    int exposure = (int)PARAM("exposure");
    glColor3ub(exposure, exposure, exposure );
    float thickness = (PARAM("thickness"));
    glBegin(GL_TRIANGLES);
    foreach (Spark * spark, mSparks) {
        if (spark->isBufferFull()) {
            QVector3D left, right;
            spark->polyPoints(left, right, thickness);

            if ((spark->mBackPolyPoints[0] - spark->mPos).length() < 1.0) {
                glVertex3d(left.x(), left.y(), left.z());
                glVertex3d(right.x(), right.y(), right.z());
                glVertex3d(spark->mBackPolyPoints[0].x(), spark->mBackPolyPoints[0].y(), spark->mBackPolyPoints[0].z());

                glVertex3d(right.x(), right.y(), right.z());
                glVertex3d(spark->mBackPolyPoints[1].x(), spark->mBackPolyPoints[1].y(), spark->mBackPolyPoints[1].z());
                glVertex3d(spark->mBackPolyPoints[0].x(), spark->mBackPolyPoints[0].y(), spark->mBackPolyPoints[0].z());
            }

            spark->mBackPolyPoints[0] = left;
            spark->mBackPolyPoints[1] = right;
        }
    }
    glEnd();
    glEnable(GL_DEPTH_TEST);

    /*
    glDisable(GL_DEPTH_TEST);
    glColor3ub(1, 1, 1 );
    glBegin(GL_POINTS);
    foreach (Spark * spark, mSparks) {
        glVertex3d(spark->mPos.x(), spark->mPos.y(), spark->mPos.z());
    }
    glEnd();
    glEnable(GL_DEPTH_TEST);
*/
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
                mSkeleton->mSelected->setStart(pick.x(), pick.y(), pick.z());
            }
            else {
                mSkeleton->mSelected->setEnd(pick.x(), pick.y(), pick.z());
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

void GlViewWidget::setCamera(GlViewWidget::CameraState state)
{
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslated(0,0,-2);
    switch(state) {
    case CameraFront:
        break;

    case CameraSide:
        glRotatef(90,0,1,0);
        break;

    case CameraAbove:
        glRotatef(90,1,0,0);
        break;
    }
}

void GlViewWidget::setRenderMode(Skeleton::RenderMode rm)
{
    mRenderMode = rm;

    if (mRenderMode == Skeleton::RenderFinal) {
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_BLEND);
    }
    else {
        glClearColor(0,0,0.5,1);
        glDisable(GL_BLEND);
    }

    updateGL();
}

void GlViewWidget::addSpark()
{
    Spark * s = new Spark();
    // s->mPos = mSkeleton->mRoot->start();
    s->mPos = QVector3D(RandFloatNeg(), RandFloatNeg(), RandFloatNeg());

    mSparks.append(s);
}

void GlViewWidget::updateSparks()
{
    QList<Capsule> cl = mSkeleton->toCapsuleList(mSkeleton->mRoot);

    const double heat = PARAM("heat");
    const float gravity = PARAM("gravity");

    foreach (Spark* sp, mSparks) {

        for (int i=0; i < 3; i++)
        {
            sp->mPos += QVector3D(RandFloatNeg()*heat, RandFloatNeg()*heat, RandFloatNeg()*heat);

            const Capsule * closest = 0;
            float minDist = 9999.0f;
            float lowest = 100;
            float highest = -100;
            foreach (const Capsule & c, cl) {
                float dist = c.distanceFrom(sp->mPos);
                if (dist < minDist) {
                    minDist = dist;
                    closest = &c;
                }

                if (c.mStart.y() < lowest)
                {
                    lowest = c.mStart.y();
                }

                if (c.mStart.y() > highest)
                {
                    highest = c.mStart.y();
                }
            }

            const Capsule * target = closest;
            //    Capsule * target = &cl[(int(sp)/4)%cl.count()];

            {
                QVector3D capsuleCentre = Maths::closestPointOnSegment(sp->mPos, target->mStart, target->mEnd);

                sp->mNormal = (capsuleCentre-sp->mPos).normalized();


                if (minDist > 0) {
                    sp->mPos += sp->mNormal * 0.01f;
                }
                else
                {
                    sp->mPos += sp->mNormal*-0.002f ;
                     sp->mPos += (target->mEnd - target->mStart).normalized()*0.01;
                }



                sp->mPos.setY(sp->mPos.y()-gravity);

            /*    {
                    sp->mPos.setY(sp->mPos.y() - gravity);
                }*/
            }

            if (sp->mPos.y() < lowest-2.0) {
                sp->mPos.setX(sp->mPos.x() + RandFloatNeg()*0.9);
                sp->mPos.setY(highest+1.8);
                sp->mPos.setZ(sp->mPos.z() + RandFloatNeg()*0.9);
                sp->reset();
            }


            if (sp->mPos.y() > highest+2.0) {
                sp->mPos.setX(sp->mPos.x() + RandFloatNeg()*0.9);
                sp->mPos.setY(lowest-1.8);
                               sp->mPos.setZ(sp->mPos.z() + RandFloatNeg()*0.9);
                sp->reset();
            }



        }

        sp->update();
    }
}

void GlViewWidget::tick()
{
    updateSparks();
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
