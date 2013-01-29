#include "debugshapes.h"

#ifndef Q_WS_MAC
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif
DebugShapes::DebugShapes(QObject *parent) :
    QObject(parent)
{
}

void DebugShapes::add(const QVector3D &start, const QVector3D &end)
{
    Line3D line;
    line.first = start;
    line.second = end;
    mLines.append(line);
}

void DebugShapes::render() const
{
    glBegin(GL_LINES);
    glColor3f(1,0,1);
    foreach (const Line3D & line, mLines) {
        glVertex3fv((float*)&line.first);
        glVertex3fv((float*)&line.second);
    }
    glEnd();
}

void DebugShapes::clear()
{
    mLines.clear();
}
