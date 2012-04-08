#ifndef INTERVAL_H
#define INTERVAL_H

#include <QVector3D>

class Capsule
{
public:
    Capsule(const QVector3D & start, const QVector3D & end, float radius, bool isSphere);

    float distanceFrom(const QVector3D & pt) const;

    QVector3D mStart;
    QVector3D mEnd;
    float mRadius;

    /** Bones become spheres if they are too fat, we want this behaviour
     * mirrored here. @todo unify bones/capsules - no need for bone to be qobject.*/
    bool mIsSphere;
};

#endif // INTERVAL_H
