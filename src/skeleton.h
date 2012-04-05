#ifndef SKELETON_H
#define SKELETON_H

#include <QObject>
#include <QList>

class Bone;

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

    bool load(const QString & filename);

signals:
    
public slots:

private:
    Bone *findBone(Bone * root, const QString &name);
    void renderBone(Bone * bone);
    bool resolveChildren(Bone * bone);
    void linkJoints(Bone * bone);
    void applyForce(Bone * bone, double mss);
};

#endif // SKELETON_H
