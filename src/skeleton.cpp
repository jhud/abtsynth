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
#include "maths.h"
#include "branch.h"

#include <QMatrix4x4>
#ifndef Q_OS_MAC
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif
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

Branch *Skeleton::toBranchRoot(Bone *boneToFollow, const QList<Capsule> & bounds, QHash<Bone *, bool> visited, bool reverse)
{
    visited[boneToFollow] = true;

    if (!boneToFollow || !boneToFollow->joinedTo().isEmpty()) {
        // Don't allow loops
        return 0;
    }

    QMatrix4x4 xform;
    xform.setToIdentity();
    QVector3D fwd;

    if (reverse == false) {
        xform.translate(boneToFollow->start());
        fwd= (boneToFollow->end()-boneToFollow->start()).normalized();

        if (boneToFollow->name() == "back")
        {
            // hack to put heart on the left
            xform.translate(0.05,0,0);
        }

    }
    else {
        xform.translate(boneToFollow->end());
        fwd= (boneToFollow->start()-boneToFollow->end()).normalized();

        if (boneToFollow->name() == "neck")
        {
            // hack to put heart on the left
            xform.translate(0.05,0,0);
        }
    }


    xform.setColumn(0, QVector4D(fwd.y(), -fwd.z(), fwd.x(), 0));
    xform.setColumn(1, QVector4D(fwd, 0));
    xform.setColumn(2, QVector4D(fwd.x(), -fwd.x(), fwd.z(), 0));

    Branch * branch = new Branch(xform, boneToFollow->length(), boneToFollow->thicknessRatio());

    int depth = 5;

    foreach (QObject * obj, boneToFollow->children()) {
        Bone * childBone = (Bone*)obj;

        if (!visited.contains(childBone)) {
            branch->addChild(toBranchRoot(childBone, bounds, visited, false));
            branch->growRecursively(0, depth, bounds );
        }
    }

    Bone * parent = boneToFollow->parentBone();
    if (parent && !visited.contains(parent)) {
        branch->addChild(toBranchRoot(parent, bounds, visited, true));
        branch->growRecursively(0, depth, bounds );
    }

    return branch;
}

Bone *Skeleton::findBone(const QString &name)
{
    if (mRoot == 0) {
        return 0;
    }

    return findBone(mRoot, name);
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
        float m[16];
        Maths::multLookAt(m, bone->start(), bone->end(), up);
        glMultMatrixf(m);

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



