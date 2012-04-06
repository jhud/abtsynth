#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glviewwidget.h"

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
