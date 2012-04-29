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

#include <QMatrix4x4>

Capsule::Capsule(const QVector3D & start, const QVector3D & end, float radius, bool isSphere, QMatrix4x4 & transform)
    : mStart(start)
    , mEnd(end)
    , mRadius(radius)
    , mTransform(&transform)
    , mIsSphere(isSphere)
{
}



float Capsule::distanceFrom(const QVector3D &pt) const
{
    QVector3D ptInObjSpace = mTransform->map(pt-mStart);

    if (mIsSphere) {
        return (ptInObjSpace).length() - mRadius;
    }

    return Maths::distanceToSegment(ptInObjSpace, QVector3D(0,0,0), mEnd-mStart) - mRadius;
}

