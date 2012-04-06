#ifndef GLVIEWWIDGET_H
#define GLVIEWWIDGET_H

#include "skeleton.h"

#include <QGLWidget>


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

signals:
    
public slots:
    void tick(void);

private:
    double mAspectRatio;
    QVector3D screenToWorld(int x, int y, float * normalizedZ);
    QVector3D screenToWorldAlongPlane(int x, int y, float z);

    Skeleton * mSkeleton;
    bool mSelectBoneEnds;
};

#endif // GLVIEWWIDGET_H
