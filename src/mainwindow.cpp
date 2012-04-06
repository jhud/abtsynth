#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glviewwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mViewWidget = new GlViewWidget(ui->centralWidget);
    mViewWidget->setGeometry(0,0,640,480);

    mViewWidget->updateGL();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSelect_ends_triggered(bool checked)
{
    mViewWidget->setSelectBoneEnds(checked);
}
