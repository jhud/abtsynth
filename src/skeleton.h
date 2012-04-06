#ifndef SKELETON_H
#define SKELETON_H

#include <QObject>
#include <QList>
#include <GL/glu.h>

class Bone;
class QVector3D;


class Skeleton : public QObject
{
    Q_OBJECT
public:
    explicit Skeleton(QObject *parent = 0);
    virtual ~Skeleton();
    
    void resolve();

    void applyForce(double mss);

    void render();

    Bone * mRoot;
    Bone * mSelected;

    bool load(const QString & filename);

    bool selectBone(const QVector3D * pos);
signals:
    
public slots:

private:
    void findClosestBoneToPoint(Bone *root, const QVector3D *pos, float * dist, Bone ** closest);
    Bone *findBone(Bone * root, const QString &name);
    void renderBone(Bone * bone);
    void renderBoneVolume(Bone *bone, GLUquadricObj * obj);
    bool resolveChildren(Bone * bone);
    void linkJoints(Bone * bone);
    void applyForce(Bone * bone, double mss);
};

#endif // SKELETON_H
