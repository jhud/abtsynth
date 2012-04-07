#ifndef INTERVAL_H
#define INTERVAL_H

#include <QVector3D>

class Capsule
{
public:
    Capsule(const QVector3D & start, const QVector3D & end, float radius);

    float distanceFrom(const QVector3D & pt) const;

    QVector3D mStart;
    QVector3D mEnd;
    float mRadius;
};

#endif // INTERVAL_H
