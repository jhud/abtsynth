#include "branch.h"
#include "GL/gl.h"

Branch::Branch(const QMatrix4x4 &orientation, float length)
    : mMatrix(orientation)
    , mLength(length)
{
}

Branch::~Branch()
{
}

void Branch::addChild(Branch * branch)
{
    mChildren.append(branch);
}

void Branch::render() const
{
    foreach(const Branch * child, mChildren) {
        child->render();
    }


    const QVector3D pos = mMatrix.map(QVector3D(0,0,0));
    glVertex3f(pos.x(), pos.y(), pos.z());

    const QVector3D & pos2 = mMatrix.map(QVector3D(0,0,mLength));
    glVertex3f(pos2.x(), pos2.y(), pos2.z());
}

