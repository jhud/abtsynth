#ifndef BRANCH_H
#define BRANCH_H

#include <QMatrix4x4>

class Capsule;

class Branch
{
public:
    Branch(const QMatrix4x4 & orientation, float length);
    virtual ~Branch();

    void addChild(Branch * branch);

    void growRecursively(Branch * parent, int levels, const QList<Capsule> & bounds);

    void render() const;

private:
    QList<Branch*> mChildren;
    QMatrix4x4 mMatrix;
    float mLength;
};

#endif // BRANCH_H
