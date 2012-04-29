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

#include "maths.h"

#include <QVector3D>

namespace Maths {

QVector3D closestPointOnSegment
        (const QVector3D& pt, const QVector3D& from, const QVector3D& to)
{
    QVector3D diff(to - from);
    QVector3D segment(diff.normalized());
    qreal projection = QVector3D::dotProduct(pt - from, segment);

    if (projection <= 0) {
        return (from);
    }

    if (projection >= diff.length()) {
        return (to);
    }

    return from + projection * segment;
}

qreal distanceToSegment(const QVector3D& pt, const QVector3D& from, const QVector3D& to)
{
    return (closestPointOnSegment(pt, from, to) - pt).length();
}

}
