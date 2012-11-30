/* ABTSynth - 3D character poser with rigid body dynamics
   URLs: https://github.com/jhud/abtsynth
   Copyright (C) 2012, James Hudson

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see http://www.gnu.org/licenses/.
*/

#include "glviewwidget.h"
#include "skeleton.h"
#include "bone.h"
#include "mainwindow.h"
#include "common.h"
#include "maths.h"
#include "branch.h"

#include <QDebug>
#include <math.h>
#include <QTimer>
#include <GL/glu.h>
#include <qevent.h>
#include <QMessageBox>


#define RandRangeFloat(x, y) (float(rand()%RAND_MAX)/RAND_MAX * ((y)-(x)) - ((x)*0.5f))

GLUquadricObj * qo;
QVector3D pick;
float pickedZ;

GlViewWidget::GlViewWidget(QWidget *parent) :
    QGLWidget(parent)
  , mSelectBoneEnds(false)
  , mBloodVessels(0)
{
    qo = gluNewQuadric();

    setFocus();

    mSkeleton = new Skeleton(this);
    if (!mSkeleton->load("../abtsynth/data/guy.xml")) {
        new QMessageBox(QMessageBox::Critical, "Error", "Could not load skeleton.");
    }
    else {
        mSkeleton->resolve();

        QTimer * timer;
        timer = new QTimer(this);
        timer->setSingleShot(false);
        connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
        timer->start(50);

        for (int i=0; i<0; i++) {
            addSpark();
        }

        rebuildBloodVessels();
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

    foreach (Ribbon * ribbon, mRibbons)
    {
        ribbon->draw();
    }

        glColor3f( 1.0f, 0.0f, 0.0f );
    glBegin(GL_LINES);
    mBloodVessels->render();
    glEnd();
        glColor3f( 1.0f, 1.0f, 1.0f );

    /*   glBegin(GL_TRIANGLES);
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
    glEnd();*/


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

               rebuildBloodVessels();
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
    Ribbon * s = new Ribbon(this);
    // s->mPos = mSkeleton->mRoot->start();
    s->update(QVector3D(RandFloatNeg(), RandFloatNeg(), RandFloatNeg()), QVector3D(0,0,0));

    mRibbons.append(s);
}

void GlViewWidget::updateSparks()
{
    QList<Capsule> cl = mSkeleton->toCapsuleList(mSkeleton->mRoot);

    // todo - make these into veins, with a stack. Then write a separate 2D outliner

    const double heat = PARAM("heat");
    const float gravity = PARAM("gravity");
    const float width = PARAM("thickness")*0.5f;
    const float thicknessModulation = (PARAM("thicknessModulation"))*0.2f;
    const float attraction = PARAM("attraction")*0.1;
    const int lifetime = PARAM("lifetime")*10;
    const float exposure = PARAM("exposure");

    foreach (Ribbon* sp, mRibbons) {

        QVector3D newPos = sp->pos();

        newPos += QVector3D(RandFloatNeg()*heat, RandFloatNeg()*heat, RandFloatNeg()*heat * (mClampDepth ? 0:1));

        const Capsule * closest = 0;
        float minDist = 9999.0f;
        float lowest = 100;
        float highest = -100;
        int idx =0;
        int i=0;
        foreach (const Capsule & c, cl) {
            float dist = c.distanceFrom(newPos);
            if (dist < minDist) {
                minDist = dist;
                closest = &c;
                idx = i;
            }
            i++;

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

        if ((rand()&63) == 0)
        {
        target = &cl[rand()%cl.size()];
}

        QVector3D closestPoint;
        float dist = target->distanceFrom(newPos, &closestPoint);

        if (mClampDepth) {
            closestPoint.setZ(0);
            newPos.setZ(0);
        }



        QVector3D normal = target->normal(newPos);

        if (!closest->mIsSphere)
        {
        newPos -= normal * 0.0005;
        }
        newPos += (target->mEnd-closest->mStart).normalized() * attraction;

        //newPos = closestPoint;

        newPos += QVector3D(0, -gravity, 0);


        if (newPos.y() < lowest-1.0) {
            newPos.setX(newPos.x() + RandFloatNeg()*0.9);
            newPos.setY(highest+0.8);
            newPos.setZ(newPos.z() + RandFloatNeg()*0.9);
            sp->reset();
        }


        if (newPos.y() > highest+1.0) {
            newPos.setX(newPos.x() + RandFloatNeg()*0.9);
            newPos.setY(lowest-0.8);
            newPos.setZ(newPos.z() + RandFloatNeg()*0.9);
            sp->reset();
        }

        if (mClampDepth) {
            newPos.setZ(0);
        }

        QVector3D colour = QVector3D(normal.x() * exposure,
                                     normal.y() * exposure,
                                     normal.z() * exposure) * (1.0f/255.0f);

        static float lfo = 0;
        lfo += 0.05;
        sp->setWidth(width + sin(lfo) * thicknessModulation);
        sp->update(newPos, colour);
    }
}

void GlViewWidget::save(const QString &filename)
{
    mSkeleton->save(filename);
}

void GlViewWidget::clampDepth(bool clamp)
{
    mClampDepth = clamp;
}

void GlViewWidget::rebuildBloodVessels()
{
    delete mBloodVessels;
    QList<Capsule> cl = mSkeleton->toCapsuleList(mSkeleton->mRoot);
    mBloodVessels = mSkeleton->toBranchRoot(mSkeleton->findBone("back"), cl);
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
