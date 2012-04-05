#ifndef GLVIEWWIDGET_H
#define GLVIEWWIDGET_H

#include <QGLWidget>

class Skeleton;

/**
 * A viewport onto a 3D space
 */
class GlViewWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GlViewWidget(QWidget *parent = 0);
    
    void initializeGL();

    void resizeGL( int w, int h );

    void paintGL();

    void mouseMoveEvent(QMouseEvent *);

    void mouseDoubleClickEvent(QMouseEvent *);

    void keyPressEvent(QKeyEvent *);

signals:
    
public slots:
    void tick(void);

private:
    double mAspectRatio;
    QVector3D screenToWorld(int x, int y, bool ray);

    Skeleton * mSkeleton;
};

#endif // GLVIEWWIDGET_H
