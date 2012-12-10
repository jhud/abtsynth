#ifndef BRANCHFOLLOWINGRIBBON_H
#define BRANCHFOLLOWINGRIBBON_H

#include "ribbon.h"

class Branch;

class BranchFollowingRibbon : public Ribbon
{
    Q_OBJECT
public:
    explicit BranchFollowingRibbon(Branch * root, QObject * parent = 0);

    bool takeRandomChild(const QVector3D &colour);

    virtual void reset();

private:
    Branch * mCurrentBranch;
    Branch * mOriginalRoot;
};

#endif // BRANCHFOLLOWINGRIBBON_H
