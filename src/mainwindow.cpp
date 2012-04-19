#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glviewwidget.h"
#include "skeleton.h"

#include <QDebug>

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

