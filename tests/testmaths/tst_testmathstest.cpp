#include <QString>
#include <QtTest>

#include "../../src/maths.h"

class TestmathsTest : public QObject
{
    Q_OBJECT
    
public:
    TestmathsTest() {}
    
private Q_SLOTS:
    void testRandomFloat()
    {
        float average = 0;
        for (int i=0; i<100; i++) {
            QVERIFY2(Maths::randomUnit() >= 0 && Maths::randomUnit() < 1.0f, "Out of range");
            average = Maths::randomUnit();
        }

        QVERIFY2(average > 0.25 && average < 0.75, "Output is skewed.");
    }

    void testRandomUnitVector3D()
    {
        QVector3D sum;
        for (int i=0; i<100; i++) {
            QVector3D vec = Maths::randomUnitVector3D();

            QVERIFY2(vec.x() >= 0 && vec.x() < 1.0f, "Out of range");
            QVERIFY2(vec.y() >= 0 && vec.y() < 1.0f, "Out of range");
            QVERIFY2(vec.z() >= 0 && vec.z() < 1.0f, "Out of range");

            sum = sum+vec;
        }

        QVERIFY2(sum.x() > 25 && sum.x() < 75, "Output skewed");
        QVERIFY2(sum.y() > 25 && sum.y() < 75, "Output skewed");
        QVERIFY2(sum.z() > 25 && sum.z() < 75, "Output skewed");
    }
};


QTEST_APPLESS_MAIN(TestmathsTest)

#include "tst_testmathstest.moc"
