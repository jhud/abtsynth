#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QDebug>

#define private public
#include "../../../src/bone.h"
#undef private

class Test_Bone : public QObject
{
    Q_OBJECT
    
public:
    Test_Bone();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testResolveLength();
    void testResolveDirection();
    void testDistanceFromPoint();
    void testDistanceFromPoint2();
};

Test_Bone::Test_Bone()
{
        qDebug() << "Setting up test case.";
}

void Test_Bone::initTestCase()
{
    qDebug() << "Setting up test case.";
}

void Test_Bone::cleanupTestCase()
{
}

void Test_Bone::testResolveLength()
{
    Bone bone;

    bone.setLength(2.0);

    int sanity = 0;

    while(!bone.resolve()) {
        QVERIFY2(sanity < 100, "Bone never resolved.");
        sanity++;
    }
}

void Test_Bone::testResolveDirection()
{
    Bone bone;

    bone.setLength(2.0);
    bone.mPos[1]->setX(1.5f);

    int sanity = 0;

    while(!bone.resolve()) {
        QVERIFY2(sanity < 100, "Bone never resolved.");
        sanity++;
    }

   // QVERIFY2(bone.mPos[1]->x() > 1.99, "bone is facing wrong way");
}

void Test_Bone::testDistanceFromPoint()
{
    Bone bone;
    const QVector3D pt(75, 5, 0);

    bone.mThicknessRatio = 0;
    bone.setLength(100.0);
    bone.mPos[0]->setX(10.0);
    bone.mPos[1]->setX(110.0);
    bone.mPos[0]->setY(0.0);
    bone.mPos[1]->setY(0.0);
    bone.mPos[0]->setZ(0.0);
    bone.mPos[1]->setZ(0.0);

    double dist = bone.distanceFromPoint(&pt);

    QCOMPARE(dist, 5.0);
}

void Test_Bone::testDistanceFromPoint2()
{
    Bone bone;
    const QVector3D pt(120, 0, 0);

    bone.mThicknessRatio = 0;
    bone.setLength(100.0);
    bone.mPos[0]->setX(10.0);
    bone.mPos[1]->setX(110.0);
    bone.mPos[0]->setY(0.0);
    bone.mPos[1]->setY(0.0);
    bone.mPos[0]->setZ(0.0);
    bone.mPos[1]->setZ(0.0);

    double dist = bone.distanceFromPoint(&pt);

    QCOMPARE(dist, 10.0);
}

QTEST_MAIN(Test_Bone)

#include "tst_test_bone.moc"
