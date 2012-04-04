#include "glviewwidget.h"

#include <GL/glu.h>

GlViewWidget::GlViewWidget(QWidget *parent) :
    QGLWidget(parent)
{
}

void GlViewWidget::initializeGL()
{
    glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );
}

void GlViewWidget::resizeGL( int w, int h )
{
    if ( h == 0 )
        h = 1;

    // Set the viewport to window dimensions
    glViewport( 0, 0, w, h );

    // reset the coordinate system
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    mAspectRatio = double( w ) / double( h );

    gluPerspective(90.0, mAspectRatio, 0.5, 500);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void GlViewWidget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT );
    glColor3f( 1.0f, 0.0f, 0.0f );

    glBegin(GL_LINES);
    glVertex3f(0,0,-10);
    glVertex3f(1,1,-20);
    glEnd();
}

