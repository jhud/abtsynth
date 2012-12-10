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
void multLookAt (float m[],
                 const QVector3D & ieye,
                 const QVector3D & iat,
                 const QVector3D & iup)
{
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
}

// End of namespace
}
