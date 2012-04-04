#include "skeleton.h"
#include "bone.h"

#include <GL/gl.h>
#include <QDebug>
#include <QList>



Skeleton::Skeleton(QObject *parent) :
    QObject(parent)
{
    mRoot = new Bone(0);
    mRoot->setLength(1.0f);

    Bone * back = new Bone(mRoot);
    back->setLength(1.0f);

    Bone * leftThigh = new Bone(back);
    Bone * leftShin = new Bone(leftThigh);

    leftThigh->setLength(1.0f);
    leftShin->setLength(1.0f);

    Bone * rightThigh = new Bone(back);
    rightThigh->mPos[1].setX(-1.0);
    rightThigh->mPos[1].setY(1.0);
    rightThigh->setLength(1.0f);

    Bone * rightShin = new Bone(rightThigh);
    rightShin->setLength(1.0f);


    Bone * leftArm = new Bone(mRoot);
    leftArm->setLength(0.5f);
    leftArm->mPos[1].setX(-0.2);
    leftArm->mPos[1].setY(-0.3);

    Bone * leftForearm = new Bone(leftArm);
    leftForearm->setLength(0.5f);

    Bone * rightArm = new Bone(mRoot);
    Bone * rightForearm = new Bone(rightArm);
    rightForearm->mPos[1].setX(-1.0);
    rightForearm->mPos[1].setY(-0.5);
    rightArm->setLength(0.5f);
    rightForearm->setLength(0.5f);


}

Skeleton::~Skeleton()
{
    delete mRoot;
}

void Skeleton::resolve()
{
    while (!resolveChildren(mRoot)) {
    }
}

void Skeleton::applyForce(double mss)
{
    foreach (QObject * obj, mRoot->children()) {
        Bone * childBone = (Bone*)obj;

        applyForce(childBone, mss);
    }
}

void Skeleton::render()
{
    glBegin(GL_LINES);

    renderBone(mRoot);

    glEnd();
}

void Skeleton::separate(Bone *bone, QList<Bone *> * list)
{
    list->append(bone);

    foreach (QObject * obj, bone->children()) {
        Bone * childBone = (Bone*)obj;

        separate(childBone, list);
    }

    foreach (Bone * val1, *list) {

        foreach (Bone * val2, *list) {
            if (val1 != val2 && val2!=mRoot) {
                for (int i=0; i<2; i++) {
                    QVector3D v = (val1->mPos[1]-val2->mPos[1]);
                    v.normalize();
                    val2->mPos[1] += v*0.07;
                }
            }
        }
    }
}

void Skeleton::renderBone(Bone *bone)
{
    foreach (QObject * obj, bone->children()) {
        Bone * childBone = (Bone*)obj;

        renderBone(childBone);
    }

    for (int i=0; i<2; i++) {
        const QVector3D * pos = &bone->mPos[i];
        glVertex3f(pos->x(), pos->y(), pos->z());
    }
}



bool Skeleton::resolveChildren(Bone *bone)
{
    bool isResolved = bone->resolve();

    foreach (QObject * obj, bone->children()) {
        Bone * childBone = (Bone*)obj;

        childBone->mPos[0] = bone->mPos[1];

        isResolved = isResolved && resolveChildren(childBone);
    }

    return isResolved;
}

void Skeleton::applyForce(Bone *bone, double mss)
{
    foreach (QObject * obj, bone->children()) {
        Bone * childBone = (Bone*)obj;
        applyForce(childBone, mss);
    }

    bone->mVel[0].setY(mss);
    bone->mVel[1].setY(mss);

    bone->mPos[0] += bone->mVel[0];
    bone->mPos[1] += bone->mVel[1];
}

