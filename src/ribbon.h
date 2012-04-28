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
    void update(const QVector3D & newPoint, const QVector3D &newColour);
    void draw();
    void reset();
    void setWidth(float width) { mRibbonWidth = width; }

    QVector3D pos();

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
    QVector3D mColour;
    QVector3D mOldColour;
};

#endif // RIBBON_H
