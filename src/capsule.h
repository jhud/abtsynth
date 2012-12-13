#ifndef INTERVAL_H
#define INTERVAL_H

#include <QVector3D>

class QMatrix4x4;
class QVector2D;

class Capsule
{
public:
    Capsule(const QVector3D & start, const QVector3D & end, float radius, bool isSphere, QMatrix4x4 & transform, QMatrix4x4 &invertedTransform);

    float distanceFrom(const QVector3D & pt, QVector3D *nearestPoint = 0) const;

    float distanceFrom(const QVector2D &pt2, QVector2D *nearestPoint2 = 0) const;

    QVector3D normal(const QVector3D &pt) const;

    QVector2D normal(const QVector2D &pt) const;

    QVector3D mStart;
    QVector3D mEnd;
    float mRadius;

    QMatrix4x4 * mTransform;
    QMatrix4x4 * mInverseTransform;

    /** Bones become spheres if they are too fat, we want this behaviour
     * mirrored here. @todo unify bones/capsules - no need for bone to be qobject.*/
    bool mIsSphere;
};

#endif // INTERVAL_H
