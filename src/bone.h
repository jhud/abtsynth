#ifndef BONE_H
#define BONE_H

#include <QObject>
#include <QVector3D>

class Bone : public QObject
{
    Q_OBJECT
public:
    explicit Bone(QObject *parent = 0);
    
    void setLength(double length);

    QVector3D mPos[2];
    QVector3D mVel[2];

    bool resolve();

signals:
    
public slots:
    
private:
    double mLength;
};

#endif // BONE_H
