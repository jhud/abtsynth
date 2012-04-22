#ifndef SPARK_H
#define SPARK_H

#include <QVector3D>

class Ribbon;

class Spark
{
public:
    static const int BufferSize = 20;

public:
    Spark();

    int mLifetime;
    QVector3D mPos;
    QVector3D mNormal;

    void update();

    void reset();

    inline bool isBufferFull() { return mNumWritten >= BufferSize; }

    inline int numWritten() { return (mNumWritten); }

    const QVector3D & oldestPos() const;

    void polyPoints(QVector3D & p1, QVector3D & p2, float thickness);

    QVector3D mBackPolyPoints[2];

    Ribbon * mRibbon;

private:
    QVector3D mRollingBuffer[BufferSize];

    int mNumWritten;
};

#endif // SPARK_H
