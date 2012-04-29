/* ABTSynth - 3D character poser with rigid body dynamics
   URLs: https://github.com/jhud/abtsynth
   Copyright (C) 2012, James Hudson

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see http://www.gnu.org/licenses/.
*/

#include "bone.h"
#include "common.h"
#include "maths.h"

#include <QtGlobal>
#include <QDomDocument>
#include <QDebug>
#include <QMatrix4x4>

Bone::Bone(QObject *parent)
    : QObject(parent)
    , mLinkedToOtherBone(false)
{
    mPos[0] = new QVector3D(RandFloat(), RandFloat(), RandFloat()); //@todo mem leak
    mPos[1] = new QVector3D(RandFloat(), RandFloat(), RandFloat());

    mTransform = new QMatrix4x4();
    mTransform->setToIdentity();

    mTransformInverse = new QMatrix4x4();
    *mTransformInverse = mTransform->inverted();
}

void Bone::setLength(double length)
{
    mLength = length;
}

double *Bone::transform()
{
    return (double*)mTransform->data();
}

bool Bone::parse(QDomNode *node)
{
    QDomElement elem = node->toElement();
    QString val;
    bool ok = false;

    val = elem.attribute("length", "1.0");
    setLength(val.toDouble(&ok));
    if (ok == false) {
        setLength(1.0);
    }

    val = elem.attribute("thickness", "0.1");
    mThicknessRatio = val.toDouble(&ok);
    if (ok == false) {
        mThicknessRatio = 0.1;
    }

    val = elem.attribute("scalex", "1.0");
    float valFloat = val.toFloat(&ok);
    if (ok == true) {
        mTransform->scale(valFloat, 1.0f, 1.0f);
    }

    val = elem.attribute("scaley", "1.0");
    valFloat = val.toFloat(&ok);
    if (ok == true) {
        mTransform->scale(1.0f, valFloat, 1.0f);
    }

    val = elem.attribute("scalez", "1.0");
    valFloat = val.toFloat(&ok);
    if (ok == true) {
        mTransform->scale(1.0f, 1.0f, valFloat);
    }

    mName = elem.attribute("name", "unknown");
    mJoinedTo = elem.attribute("join", "");

    QDomNode n = node->firstChild();
    while( !n.isNull() )
    {
      QDomElement e = n.toElement();
      if( !e.isNull() )
      {
        if( e.tagName() == "bone" )
        {
            Bone * child = new Bone(this);
            child->parse(&n);
        }
      }

      n = n.nextSibling();
    }

    *mTransformInverse = mTransform->inverted();

    return true;
}

void Bone::setStart(double x, double y, double z)
{
    mPos[0]->setX(x);
    mPos[0]->setY(y);
    mPos[0]->setZ(z);
}

void Bone::setStart(QVector3D * start)
{
    Q_ASSERT(start != mPos[1]);
    mPos[0] = start;
}

void Bone::setEnd(QVector3D *link)
{
    Q_ASSERT(link != mPos[0]);

    if (mLinkedToOtherBone == false && mPos[1] != link) {
        delete mPos[1];
        mLinkedToOtherBone = true;
    }
    mPos[1] = link;
}

void Bone::setEnd(double x, double y, double z)
{
    mPos[1]->setX(x);
    mPos[1]->setY(y);
    mPos[1]->setZ(z);
}

float Bone::distanceFromPoint(const QVector3D *point)
{
    return Maths::distanceToSegment(*point, *mPos[0], *mPos[1]);
}

bool Bone::resolve()
{
    QVector3D * s;
    QVector3D * e;

    if (rand()&3) {
        s = mPos[0];
        e = mPos[1];
    }
    else {
        s = mPos[1];
        e = mPos[0];
    }

    if (s == e) {
        e->setX(e->x()+0.0001);
    }

    QVector3D diff(*s-*e);
    double actualLength = diff.length();
    double lengthDiff = actualLength - mLength;

    if (qAbs(lengthDiff) < 0.05) {
        return true;
    }

    *e = *s - diff.normalized() * (actualLength - lengthDiff*0.95);

    return false;
}

Capsule Bone::toCapsule()
{
    return Capsule(*mPos[0], *mPos[1], mLength * mThicknessRatio * 0.5, mThicknessRatio >= 1.0f, *mTransformInverse);
}
