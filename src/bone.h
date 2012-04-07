#ifndef BONE_H
#define BONE_H

#include <QObject>
#include <QVector3D>

#include "capsule.h"

class QDomNode;

class Bone : public QObject
{
    Q_OBJECT
public:
    explicit Bone(QObject *parent = 0);
    
    void setLength(double length);
    double length() { return mLength; }

    double thicknessRatio() { return mThicknessRatio; }

    QString name() const { return mName; }
    QString joinedTo() const { return mJoinedTo; }

    bool parse(QDomNode * node);

    /**
     * Replace start of bone with a new reference. Old instance is destroyed if the bone owned it.
     */
    void setStart(QVector3D * start);
    void setStart(double x, double y, double z);
    QVector3D & start() { return *mPos[0];}
    QVector3D & end() { return *mPos[1];}

    /**
     * Replace end of bone with a new reference. Old instance is destroyed if the bone owned it.
     */
    void setEnd(QVector3D * link);
    void setEnd(double x, double y, double z);

    float distanceFromPoint(const QVector3D *point);

    bool resolve();

    Capsule toCapsule();

signals:
    
public slots:
    
private:
    double mLength;
    double mThicknessRatio;
    QString mName;
    QString mJoinedTo;
    bool mLinkedToOtherBone;

    QVector3D * mPos[2];
};

#endif // BONE_H
