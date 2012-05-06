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

#include "skeleton.h"
#include "bone.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <QDebug>
#include <QList>
#include <QDomDocument>
#include <QFile>



Skeleton::Skeleton(QObject *parent)
    : QObject(parent)
    , mRoot(0)
    , mSelected(0)
    , mRenderMode(RenderSolid)
    , mDomFile(0)
{
}

Skeleton::~Skeleton()
{
    delete mRoot;
    delete mDomFile;
}

void Skeleton::resolve()
{
    int numIterations=0;
    while (!resolveChildren(mRoot) && numIterations < 1000) {
        numIterations++;
    }

    if (numIterations == 1000) {
        qDebug() << "Could not resolve skeleton.";
    }
}

QList<Capsule> Skeleton::toCapsuleList(Bone * root)
{
    QList<Capsule> intervals;

    if (!root) {
        return intervals;
    }

    intervals.append(root->toCapsule());

    foreach (QObject * obj, root->children()) {
        Bone * childBone = (Bone*)obj;

        intervals.append(toCapsuleList(childBone));
    }

    return intervals;
}

void Skeleton::render(RenderMode r)
{
    if (!mRoot) {
        return;
    }

    if (r == RenderSolid) {
        GLUquadricObj * obj = gluNewQuadric();
        renderBoneVolume(mRoot, obj);
        gluDeleteQuadric(obj);
    }
    else if (r == RenderStickman){
        glBegin(GL_LINES);

        renderBone(mRoot);

        glEnd();
    }
}

bool Skeleton::load(const QString &filename)
{
    delete mDomFile;
    mDomFile = new QDomDocument( "Skeleton");

    QFile file( filename );
    if( !file.open(QFile::ReadOnly ) ) {
        return false;
    }
    if( !mDomFile->setContent( &file ) )
    {
        file.close();
        return false;
    }
    file.close();

    QDomElement root = mDomFile->documentElement();
    if( root.tagName() != "skeleton" ) {
        return false;
    }

    QDomNode n = root.firstChild();
    while( !n.isNull() )
    {
        QDomElement e = n.toElement();
        if( !e.isNull() )
        {
            if( e.tagName() == "bone" )
            {
                mRoot = new Bone(0);
                mRoot->parse(&n);
            }
        }

        n = n.nextSibling();
    }

    linkJoints(mRoot);

    return true;
}


bool Skeleton::save(const QString &filename)
{

    QFile file( filename );
    if( !file.open(QFile::WriteOnly ) ) {
        return false;
    }

    writeBoneGraph(mRoot);

    file.write(mDomFile->toByteArray());

    file.close();

    return true;
}


bool Skeleton::selectBone(const QVector3D *pos)
{
    float dist = 999.0f;
    mSelected = 0;

    findClosestBoneToPoint(mRoot, pos, &dist, &mSelected);

    if (mSelected) {
        return true;
    }

    return false;
}

void Skeleton::findClosestBoneToPoint(Bone * root, const QVector3D *pos, float * dist, Bone ** closest)
{
    float newDist = root->distanceFromPoint(pos);
    if (newDist < *dist) {
        *dist = newDist;
        *closest = root;
    }

    foreach (QObject * obj, root->children()) {
        Bone * childBone = (Bone*)obj;

        findClosestBoneToPoint(childBone, pos, dist, closest);
    }
}

Bone *Skeleton::findBone(Bone * root, const QString &name)
{
    if (name == root->name()) {
        return root;
    }

    foreach (QObject * obj, root->children()) {
        Bone * childBone = (Bone*)obj;

        Bone * found = findBone(childBone, name);
        if (found) {
            return found;
        }
    }

    return 0;
}

void Skeleton::renderBone(Bone *bone)
{
    foreach (QObject * obj, bone->children()) {
        Bone * childBone = (Bone*)obj;

        renderBone(childBone);
    }

    const QVector3D & pos = bone->start();
    glVertex3f(pos.x(), pos.y(), pos.z());

    const QVector3D & pos2 = bone->end();
    glVertex3f(pos2.x(), pos2.y(), pos2.z());
}

// from opengl.org
/*
    multLookAt -- Create a matrix to make an object, such as
        a camera, "look at" another object or location, from
        a specified position.

    Algorithm:
        The desired transformation is obtained with this 4x4 matrix:
            |  [xaxis] 0  |
            |    [up]  0  |
            |   [-at]  0  |
            |   [eye]  1  |
        Where 'xaxis', 'up' and 'at' are the new X, Y, and Z axes of
        the transforned object, respectively, and 'eye' is the input
        new location of the transformed object.

    Assumptions:
        The camera geometry is defined to be facing
        the negative Z axis.

    Usage:
        multLookAt creates a matrix and multiplies it onto the
        current matrix stack. Typical usage would be as follows:

            glMatrixMode (GL_MODELVIEW);
            // Define the usual view transformation here using
            //   gluLookAt or whatever.
            glPushMatrix();
            multLookAt (orig[0], orig[1], orig[2],
                at[0], at[1], at[2],
                up[0], up[1], up[2]);
            // Define "camera" object geometry here
            glPopMatrix();

    Warning: Results become undefined as (at-eye) approaches
        coincidence with (up).
*/
static void multLookAt (const QVector3D & ieye,
                        const QVector3D & iat,
                        const QVector3D & iup)
{
    float m[16];

    // Compute our new look at vector, which will be
    //   the new negative Z axis of our transformed object.
    QVector3D atVec;
    atVec = ieye-iat;
    atVec.normalize();

    QVector3D xVec = QVector3D::crossProduct(atVec, iup);
    xVec.normalize();

    // Calculate the new up vector, which will be the
    //   positive Y axis of our transformed object. Note
    //   that it will lie in the same plane as the new
    //   look at vector and the old up vector.
    QVector3D upVec = QVector3D::crossProduct(xVec, atVec);

    // Account for the fact that the geometry will be defined to
    //   point along the negative Z axis.
    atVec *=  -1.f;

    // Fill out the rest of the 4x4 matrix
    m[0] = xVec.x();
    m[1] = xVec.y();
    m[2] = xVec.z();
    m[3] = 0.f;     // xaxis is m[0..2]
    m[4] = upVec.x();
    m[5] = upVec.y();
    m[6] = upVec.z();
    m[7] = 0.f;     // up is m[4..6]
    m[8] = atVec.x();
    m[9] = atVec.y();
    m[10] = atVec.z();
    m[11] = 0.f;    // -at is m[8..10]
    m[12] = ieye.x();
    m[13] = ieye.y();
    m[14] = ieye.z();
    m[15] = 1.f;

    // Multiply onto current matrix stack.
    glMultMatrixf (m);
}



void Skeleton::renderBoneVolume(Bone *bone, GLUquadricObj * quadric)
{
    foreach (QObject * obj, bone->children()) {
        Bone * childBone = (Bone*)obj;

        renderBoneVolume(childBone, quadric);
    }

    float width = bone->length()*bone->thicknessRatio()*0.5f;
    const QVector3D up(0 ,1 ,0);

    glPushMatrix();
    {
        multLookAt(bone->start(), bone->end(), up);

        if (bone->thicknessRatio() < 1.0f) {
            glPushMatrix();
            {
                glMultMatrixd(bone->transform());
                gluCylinder(quadric, width, width, bone->length(), 18, 1);
            }
            glPopMatrix();


            glPushMatrix();
            {
                glMultMatrixd(bone->transform());
                glTranslatef(0, 0, bone->length());
                gluSphere(quadric, width,12,12);
            }
            glPopMatrix();
        }

        glPushMatrix();
        {
            glMultMatrixd(bone->transform());
           // glTranslatef(bone->start().x(), bone->start().y(), bone->start().z());
            gluSphere(quadric, width,12,12);
        }
        glPopMatrix();
    }
    glPopMatrix();
}

bool Skeleton::resolveChildren(Bone *bone)
{
    if (!mRoot) {
        return true;
    }

    bool isResolved = bone->resolve();

    foreach (QObject * obj, bone->children()) {
        Bone * childBone = (Bone*)obj;

        isResolved = isResolved && resolveChildren(childBone);
    }

    return isResolved;
}

void Skeleton::linkJoints(Bone *bone)
{
    if (!bone) {
        return;
    }

    QString joinedTo(bone->joinedTo());
    if (!joinedTo.isEmpty()) {
        Bone * found = findBone(mRoot, joinedTo);
        if (!found) {
            qDebug() << "WARNING: bone" << joinedTo << "not found!";
            return;
        }
        bone->setEnd(&found->start());
    }

    foreach (QObject * obj, bone->children()) {
        Bone * childBone = (Bone*)obj;
        childBone->setStart(&bone->end());
        linkJoints(childBone);
    }
}

void Skeleton::writeBoneGraph(Bone *bone)
{
    bone->writePose(*mDomFile);

    foreach (QObject * obj, bone->children()) {
        Bone * childBone = (Bone*)obj;

        writeBoneGraph(childBone);
    }
}



