#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QDebug>

#include "../../../src/bone.h"

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
    bone.mPos[1].setX(1.5f);

    int sanity = 0;

    while(!bone.resolve()) {
        QVERIFY2(sanity < 100, "Bone never resolved.");
        sanity++;
    }

    QVERIFY2(bone.mPos[1].x() > 1.99, "bone is facing wrong way");
}

QTEST_MAIN(Test_Bone)

#include "tst_test_bone.moc"
