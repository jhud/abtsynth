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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glviewwidget.h"
#include "skeleton.h"

#include <QDebug>
#include <QFileDialog>

MainWindow * gMW=0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    Q_ASSERT(!gMW);
    gMW = this;

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


MainWindow & MainWindow::get()
{
    Q_ASSERT(gMW);
    return *gMW;
}


void MainWindow::setStatusText(const QString & text)
{
    ui->statusBar->showMessage(text);
}

void MainWindow::on_actionSelect_ends_triggered(bool checked)
{
    ui->glViewContainer->setSelectBoneEnds(checked);
}

void MainWindow::on_actionFrontCamera_triggered()
{
    ui->glViewContainer->setCamera(GlViewWidget::CameraFront);
}

void MainWindow::on_actionSideCamera_triggered()
{
    ui->glViewContainer->setCamera(GlViewWidget::CameraSide);
}

void MainWindow::on_actionAboveCamera_triggered()
{
    ui->glViewContainer->setCamera(GlViewWidget::CameraAbove);
}

void MainWindow::on_actionRenderSolid_triggered()
{
    ui->glViewContainer->setRenderMode(Skeleton::RenderSolid);
}

void MainWindow::on_actionRenderStickman_triggered()
{
    ui->glViewContainer->setRenderMode(Skeleton::RenderStickman);
}

void MainWindow::on_actionRender_triggered()
{
    ui->glViewContainer->setRenderMode(Skeleton::RenderFinal);
}

float MainWindow::param(const QString & name)
{
    QDoubleSpinBox* obj = ui->parametersDock->findChild<QDoubleSpinBox*>(name);

    if (obj) {
        return obj->value();
    }
    else {
        QDial* dial = ui->parametersDock->findChild<QDial*>(name);
        if (dial) {
            return (float)dial->value() / 100.0f;
        }
        else {
            qDebug() << "ERROR: parameter" << name << "not found!";
            return 0;
        }
    }
}


void MainWindow::on_action_Save_as_triggered()
{
    QString s = QFileDialog::getSaveFileName(this, "Save file", QString(), "Skeletons (*.xml)");

    if (!s.isEmpty()) {
        ui->glViewContainer->save(s);
    }
}

void MainWindow::on_actionClamp_Depth_triggered(bool checked)
{
    ui->glViewContainer->clampDepth(checked);
}
