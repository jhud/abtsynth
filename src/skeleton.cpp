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

    if (true) {
        GLUquadricObj * obj = gluNewQuadric();
        renderBoneVolume(mRoot, obj);
        gluDeleteQuadric(obj);
    }
    else {
    glBegin(GL_LINES);

    renderBone(mRoot);

    glEnd();
    }
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

    if (bone->thicknessRatio() < 1.0f) {
    glPushMatrix();
    {
        const QVector3D up(0 ,1 ,0);
        multLookAt(bone->start(), bone->end(), up);
        gluCylinder(quadric, width, width, bone->length(), 6, 2);
    }
    glPopMatrix();


    glPushMatrix();
    {
        glTranslatef(bone->end().x(), bone->end().y(), bone->end().z());
        gluSphere(quadric, width,12,12);
    }
    glPopMatrix();
    }

    glPushMatrix();
    {
        glTranslatef(bone->start().x(), bone->start().y(), bone->start().z());
        gluSphere(quadric, width,12,12);
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

