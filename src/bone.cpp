#include "bone.h"

#include <QtGlobal>

Bone::Bone(QObject *parent) :
    QObject(parent)
{
}

void Bone::setLength(double length)
{
    mLength = length;
}

bool Bone::resolve()
{
    if (mPos[0] == mPos[1]) {
        mPos[1].setX(mPos[1].x()+0.0001);
    }

    QVector3D diff(mPos[0]-mPos[1]);
    double actualLength = diff.length();
    double lengthDiff = actualLength - mLength;

    if (qAbs(lengthDiff) < 0.001) {
        return true;
    }

    mPos[1] = mPos[0] - diff.normalized() * (actualLength - lengthDiff*0.95);

    return false;
}
