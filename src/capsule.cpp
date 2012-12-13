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

#include "capsule.h"
#include "maths.h"

#include <QVector2D>
#include <QMatrix4x4>

Capsule::Capsule(const QVector3D & start, const QVector3D & end, float radius, bool isSphere, QMatrix4x4 & transform, QMatrix4x4 & invertedTransform)
    : mStart(start)
    , mEnd(end)
    , mRadius(radius)
    , mTransform(&transform)
    , mInverseTransform(&invertedTransform)
    , mIsSphere(isSphere)
{
}



float Capsule::distanceFrom(const QVector3D &pt, QVector3D * nearestPoint) const
{
    QVector3D ptInObjSpace = mTransform->map(pt-mStart);

    if (mIsSphere) {
        if (nearestPoint) {
            *nearestPoint = mInverseTransform->map(ptInObjSpace.normalized()*mRadius) + mStart;
        }

        return (ptInObjSpace).length() - mRadius;
    }

    if (nearestPoint) {
        QVector3D segmentPoint = Maths::closestPointOnSegment(ptInObjSpace, QVector3D(0,0,0), mEnd-mStart);
        QVector3D normal = (ptInObjSpace-segmentPoint).normalized();
        *nearestPoint = mInverseTransform->map((normal*mRadius)+segmentPoint) + mStart;
    }

    return Maths::distanceToSegment(ptInObjSpace, QVector3D(0,0,0), mEnd-mStart) - mRadius;
}

QVector3D Capsule::normal(const QVector3D &pt) const
{
    QVector3D ptInObjSpace = mTransform->map(pt-mStart);

    if (mIsSphere) {

        return (ptInObjSpace-mStart).normalized();
    }

        QVector3D segmentPoint = Maths::closestPointOnSegment(ptInObjSpace, QVector3D(0,0,0), mEnd-mStart);
        return (ptInObjSpace-segmentPoint).normalized();

}

float Capsule::distanceFrom(const QVector2D &pt2, QVector2D * nearestPoint2) const
{
    QVector3D pt(pt2.x(), pt2.y(), 0);
    QVector3D start(mStart.x(), mStart.y(), 0);
    QVector3D ptInObjSpace = mTransform->map(pt-mStart);
    ptInObjSpace.setZ(0);

    if (mIsSphere) {
        if (nearestPoint2) {
            QVector3D nearestPoint = mInverseTransform->map(ptInObjSpace.normalized()*mRadius);
            nearestPoint.setZ(0);
            nearestPoint += start;
            nearestPoint2->setX(nearestPoint.x());
            nearestPoint2->setY(nearestPoint.y());
        }

        return (ptInObjSpace).length() - mRadius;
    }

    QVector3D diff = mEnd-mStart;
    if (nearestPoint2) {
        QVector3D segmentPoint = Maths::closestPointOnSegment(ptInObjSpace, QVector3D(0,0,0), diff);
        QVector3D normal = (ptInObjSpace-segmentPoint).normalized();
        QVector3D nearestPoint = mInverseTransform->map((normal*mRadius)+segmentPoint) + start;
        nearestPoint2->setX(nearestPoint.x());
        nearestPoint2->setY(nearestPoint.y());
    }

    diff.setZ(0);
    return Maths::distanceToSegment(ptInObjSpace, QVector3D(0,0,0), diff) - mRadius;
}


QVector2D Capsule::normal(const QVector2D &pt2) const
{
    QVector3D pt(pt2.x(), pt2.y(), 0);
    QVector3D start(mStart.x(), mStart.y(), 0);
    QVector3D ptInObjSpace = mTransform->map(pt-mStart);
    ptInObjSpace.setZ(0);

    if (mIsSphere) {

        QVector3D norm = (ptInObjSpace-start).normalized();
        return QVector2D(norm.x(), norm.y());
    }

    QVector3D diff = mEnd-mStart;
        QVector3D segmentPoint = Maths::closestPointOnSegment(ptInObjSpace, QVector3D(0,0,0), diff);
        QVector3D normal = (ptInObjSpace-segmentPoint).normalized();

        return QVector2D(normal.x(), normal.y());
}
