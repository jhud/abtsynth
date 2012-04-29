#ifndef INTERVAL_H
#define INTERVAL_H

#include <QVector3D>

class QMatrix4x4;

class Capsule
{
public:
    Capsule(const QVector3D & start, const QVector3D & end, float radius, bool isSphere, QMatrix4x4 & transform);

    float distanceFrom(const QVector3D & pt) const;

    QVector3D mStart;
    QVector3D mEnd;
    float mRadius;

    QMatrix4x4 * mTransform;

    /** Bones become spheres if they are too fat, we want this behaviour
     * mirrored here. @todo unify bones/capsules - no need for bone to be qobject.*/
    bool mIsSphere;
};

#endif // INTERVAL_H
