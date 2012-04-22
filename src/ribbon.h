#ifndef RIBBON_H
#define RIBBON_H

#include <QObject>

#include <QVector3D>
#include <QList>

class Ribbon : public QObject
{
    Q_OBJECT
public:
    explicit Ribbon(QObject *parent = 0);
    
    void addRibbonCurve();
    void draw(const QVector3D & newPoint);

private:

    void set(QVector3D pStartPt, QVector3D pEndPt, QVector3D pControlPt);

    void drawNextSegment();
    QVector3D getPoint(float t, float k);

signals:
    
public slots:

private:
static const int SegmentsPerCurve = 10;

    QList<QVector3D> pts;
    float mRibbonWidth;
    int mCurrentStep;

    QVector3D mStartPt;
    QVector3D mEndPt;
    QVector3D mControlPt;
};

#endif // RIBBON_H
