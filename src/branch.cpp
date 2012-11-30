#include "branch.h"
#include "capsule.h"
#include "GL/gl.h"

Branch::Branch(const QMatrix4x4 &orientation, float length)
    : mMatrix(orientation)
    , mLength(length)
{
}

Branch::~Branch()
{
    foreach (const Branch * branch, mChildren) {
        delete branch;
    }

    mChildren.clear();
}

void Branch::addChild(Branch * branch)
{
    mChildren.append(branch);
}

void Branch::growRecursively(Branch *parent, int levels, const QList<Capsule> & bounds)
{


    if (levels > 0) {
        int numBranches = 3;
        for (int i=0; i<numBranches; i++) {
            qreal depthInsideSkin = 999;
            QMatrix4x4 m(mMatrix);
            m.translate(0,float(i)*mLength/(float(numBranches)),0);
            m.rotate(i*120, 0,1,0);
            m.rotate(20, 1,0,0);

            qreal newLength = mLength*0.75;

            if (!bounds.isEmpty()) {
                const QVector3D & pos = mMatrix.map(QVector3D(0,newLength,0));

                foreach (const Capsule & c, bounds )
                {
                    qreal df = c.distanceFrom(pos);
                    if (df < depthInsideSkin) {
                        depthInsideSkin = df;
                    }
                }

            }
            if (depthInsideSkin > 0) {
                newLength -= depthInsideSkin;
                newLength *= 0.8;

            }
            Branch * newBranch = new Branch(m, newLength);
            addChild(newBranch);

            newBranch->growRecursively(this, levels-1, bounds);


        }
    }
}

void Branch::render() const
{
    foreach(const Branch * child, mChildren) {
        child->render();
    }


    const QVector3D pos = mMatrix.map(QVector3D(0,0,0));
    glVertex3f(pos.x(), pos.y(), pos.z());

    const QVector3D & pos2 = mMatrix.map(QVector3D(0,mLength,0));
    glVertex3f(pos2.x(), pos2.y(), pos2.z());
}

