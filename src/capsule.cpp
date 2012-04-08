#include "capsule.h"
#include "maths.h"

Capsule::Capsule(const QVector3D & start, const QVector3D & end, float radius, bool isSphere)
    : mStart(start)
    , mEnd(end)
    , mRadius(radius)
    , mIsSphere(isSphere)
{
}



float Capsule::distanceFrom(const QVector3D &pt) const
{
    if (mIsSphere) {
        return (mStart-pt).length() - mRadius;
    }

    return Maths::distanceToSegment(pt, mStart, mEnd) - mRadius;
}

