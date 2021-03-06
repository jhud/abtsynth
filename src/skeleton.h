#ifndef SKELETON_H
#define SKELETON_H

#include <QObject>
#include <QList>

#if 0
#ifndef Q_OS_MAC
#include <GL/glu.h>
#else
#include <OpenGL/glu.h>
#endif
#endif

#include "capsule.h"

class Bone;
class QVector3D;
class QDomDocument;
class Branch;
class GLUquadric;
typedef class GLUquadric GLUquadricObj;


class Skeleton : public QObject
{
    Q_OBJECT
public:
    enum RenderMode {
        RenderSolid,
        RenderStickman,
        RenderFinal
    };

    explicit Skeleton(QObject *parent = 0);
    virtual ~Skeleton();
    
    void resolve();

    QList<Capsule> toCapsuleList(Bone *root);

    Branch * toBranchRoot(Bone * boneToFollow, const QList<Capsule> & bounds, QHash<Bone *, bool> visited, bool reverse);

    Bone * findBone(const QString & name);

    void render(RenderMode r);

    Bone * mRoot;
    Bone * mSelected;

    bool load(const QString & filename);
    bool save(const QString &filename);

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
    void writeBoneGraph(Bone * bone);

    RenderMode mRenderMode;

    QDomDocument * mDomFile;
};

#endif // SKELETON_H
