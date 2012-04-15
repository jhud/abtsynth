#include "spark.h"

Spark::Spark()
    : mNumWritten(0)
{
}

void Spark::update()
{
    mRollingBuffer[mNumWritten%Spark::BufferSize] = mPos;
    mNumWritten++;
}

void Spark::reset()
{
    mNumWritten = 0;
}

const QVector3D &Spark::oldestPos() const
{
    return mRollingBuffer[(mNumWritten+Spark::BufferSize-2)%Spark::BufferSize];
}

void Spark::polyPoints(QVector3D &p1, QVector3D &p2, float thickness)
{
    QVector3D edge = QVector3D::crossProduct(mNormal, (mPos-oldestPos())).normalized() * thickness;

    p1 = mPos + edge;
    p2 = mPos - edge;
}
