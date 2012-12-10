#include "branchfollowingribbon.h"

#include "branch.h"

BranchFollowingRibbon::BranchFollowingRibbon(Branch *root, QObject * parent)
    : Ribbon(parent)
    , mCurrentBranch(root)
    , mOriginalRoot(root)
{
}

bool BranchFollowingRibbon::takeRandomChild(const QVector3D & colour)
{
    if (mCurrentBranch == 0 || mCurrentBranch->children().count() == 0) {
        return false;
    }

    mCurrentBranch = mCurrentBranch->children()[ rand() % mCurrentBranch->children().count()];

    update(mCurrentBranch->startPos(), colour);

    return true;
}

void BranchFollowingRibbon::reset()
{
    Ribbon::reset();
    mCurrentBranch = mOriginalRoot;
    update(mCurrentBranch->startPos(), QVector3D(0,0,0));
}
