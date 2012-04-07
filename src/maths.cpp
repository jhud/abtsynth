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
