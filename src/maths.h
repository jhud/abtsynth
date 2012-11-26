#ifndef MATHS_H
#define MATHS_H

#include <QVector3D>

namespace Maths
{

QVector3D closestPointOnSegment
(const QVector3D& pt, const QVector3D& from, const QVector3D& to);

qreal distanceToSegment(const QVector3D& pt, const QVector3D& from, const QVector3D& to);

void multLookAt (float m[],
                 const QVector3D & ieye,
                 const QVector3D & iat,
                 const QVector3D & iup);
}
#endif // MATHS_H
