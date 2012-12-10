#ifndef BRANCH_H
#define BRANCH_H

#include <QMatrix4x4>

class Capsule;

class Branch
{
public:
    Branch(const QMatrix4x4 & orientation, float length, float thickness);
    virtual ~Branch();

    void addChild(Branch * branch);

    void growRecursively(Branch * parent, int levels, const QList<Capsule> & bounds);

    void render() const;

    const QList<Branch*> & children() const;

    const QVector3D startPos() const;

private:
    QList<Branch*> mChildren;
    QMatrix4x4 mMatrix;
    float mLength;
    float mThickness;
};

#endif // BRANCH_H
