#ifndef DEBUGSHAPES_H
#define DEBUGSHAPES_H

#include <QObject>
#include <QVector3D>
#include <QList>
#include <QPair>

class DebugShapes : public QObject
{
    Q_OBJECT
public:
    explicit DebugShapes(QObject *parent = 0);
    
    void add(const QVector3D & start, const QVector3D & end);

    void render() const;

    void clear();

signals:
    
public slots:
    
private:
    typedef QPair<QVector3D, QVector3D> Line3D;
    QList<Line3D> mLines;
};

#endif // DEBUGSHAPES_H
