#include "ribbon.h"
#include <math.h>
#include <GL/gl.h>


Ribbon::Ribbon(QObject *parent) :
    QObject(parent)
{
    mRibbonWidth = 0.2;
}


void Ribbon::draw(const QVector3D & newPoint){
    pts += newPoint;

    if (pts.count() < 3)
    {
        return;
    }

    addRibbonCurve();

    pts.removeFirst();

    glBegin(GL_QUADS);

    while (mCurrentStep <= SegmentsPerCurve)
    {
        drawNextSegment();
        mCurrentStep++;

    }
    glEnd();
}

void Ribbon::addRibbonCurve(){
    QVector3D nextPt = (QVector3D) pts[pts.size()-1];
    QVector3D curPt = (QVector3D) pts[pts.size()-2];
    QVector3D lastPt = (QVector3D) pts[pts.size()-3];

    mStartPt = (curPt + lastPt)/2;
    mEndPt = (curPt + nextPt)/2;
    mControlPt = curPt;

    mCurrentStep = 0;

}

void Ribbon::drawNextSegment()
{
    if ( mCurrentStep >= SegmentsPerCurve)
    {
        return;
    }

    float t =  (float)mCurrentStep / (float)SegmentsPerCurve;
    QVector3D p0 = getPoint( t, -mRibbonWidth );
    QVector3D p3 = getPoint( t, mRibbonWidth );

    t =  (float)(mCurrentStep+1) / (float)SegmentsPerCurve;
    QVector3D p1 = getPoint( t, -mRibbonWidth );
    QVector3D p2 = getPoint( t, mRibbonWidth );

    glVertex3fv((float*)&p0);
    glVertex3fv((float*)&p1);
    glVertex3fv((float*)&p2);
    glVertex3fv((float*)&p3);
}

QVector3D Ribbon::getPoint( float t, float k )
{
    QVector3D pt = ( 1 - t ) * ( 1 - t ) * mStartPt + 2 * t * ( 1 - t ) * mControlPt + t * t * mEndPt;
    QVector3D pd = t*(mStartPt - 2*mControlPt + mEndPt) - mStartPt + mControlPt;
    double dd = pow( QVector3D::dotProduct(pd, pd), 1.0/3.0);

    return QVector3D(pt.x() + ( k * pd.x() ) / dd,
                     pt.y() - ( k * pd.y() ) / dd,
                     pt.z() - ( k * pd.z() ) / dd);

}

