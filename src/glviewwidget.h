#ifndef GLVIEWWIDGET_H
#define GLVIEWWIDGET_H

#include "skeleton.h"

#include <QGLWidget>
#include <QList>
#include <QVector3D>

class QTimer;
class Branch;
class Ribbon;
class BranchFollowingRibbon;
class DebugShapes;

/**
 * A viewport onto a 3D space
 */
class GlViewWidget : public QGLWidget
{
    Q_OBJECT
public:
    enum CameraState {
        CameraFront,
        CameraSide,
        CameraAbove
    };

    explicit GlViewWidget(QWidget *parent = 0);
    
    void initializeGL();

    void resizeGL( int w, int h );

    void paintGL();

    void mouseMoveEvent(QMouseEvent *);

    void mousePressEvent(QMouseEvent *);

    void keyPressEvent(QKeyEvent *);

    void setSelectBoneEnds(bool select) { mSelectBoneEnds = select; }

    void setCamera(CameraState state);

    void setRenderMode(Skeleton::RenderMode rm);

    void addSpark();

    void updateSparks();

    void save(const QString & filename);

    void load(const QString & filename);

signals:
    
public slots:
    void tick(void);
    void clampDepth(bool clamp);
    void rebuildBloodVesselsNow();
private:
    void rebuildBloodVessels();

    /**
     * @brief glErrorCheck Check and log openGL errors.
     */
    void glErrorCheck();
private:
    double mAspectRatio;
    QVector3D screenToWorld(int x, int y, float * normalizedZ);
    QVector3D screenToWorldAlongPlane(int x, int y, float z);

    Branch * mBloodVessels;

    Skeleton * mSkeleton;
    Skeleton::RenderMode mRenderMode;
    bool mSelectBoneEnds;
    bool mClampDepth;
    QTimer * mPendingBloodVesselRebuild;

    QList<BranchFollowingRibbon*> mRibbons;

    QList<Ribbon*> mOutliners;

    DebugShapes * mDebugShapes;
};

#endif // GLVIEWWIDGET_H
