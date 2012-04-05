#include "skeleton.h"
#include "bone.h"

#include <GL/gl.h>
#include <QDebug>
#include <QList>
#include <QDomDocument>
#include <QFile>



Skeleton::Skeleton(QObject *parent) :
    QObject(parent)
  , mRoot(0)
{
}

Skeleton::~Skeleton()
{
    delete mRoot;
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

void Skeleton::applyForce(double mss)
{
    if (!mRoot) {
        return;
    }
    foreach (QObject * obj, mRoot->children()) {
        Bone * childBone = (Bone*)obj;

        applyForce(childBone, mss);
    }
}

void Skeleton::render()
{
    if (!mRoot) {
        return;
    }
    glBegin(GL_LINES);

    renderBone(mRoot);

    glEnd();
}

bool Skeleton::load(const QString &filename)
{
    QDomDocument doc( "Skeleton" );
    QFile file( filename );
    if( !file.open(QFile::ReadOnly ) ) {
      return false;
    }
    if( !doc.setContent( &file ) )
    {
      file.close();
      return false;
    }
    file.close();

    QDomElement root = doc.documentElement();
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


void Skeleton::applyForce(Bone *bone, double mss)
{
    if (!mRoot) {
        return;
    }
    foreach (QObject * obj, bone->children()) {
        Bone * childBone = (Bone*)obj;
        applyForce(childBone, mss);
    }

    bone->mVel[0].setY(mss);
    bone->mVel[1].setY(mss);

    //bone->mPos[0] += bone->mVel[0];
    //bone->mPos[1] += bone->mVel[1];
}

