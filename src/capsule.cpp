#include "capsule.h"
#include "maths.h"

Capsule::Capsule(const QVector3D & start, const QVector3D & end, float radius)
    : mStart(start)
    , mEnd(end)
    , mRadius(radius)
{
}



float Capsule::distanceFrom(const QVector3D &pt) const
{
    return Maths::distanceToSegment(pt, mStart, mEnd) - mRadius;
}

