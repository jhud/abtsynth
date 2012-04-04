#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glviewwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    GlViewWidget * vw = new GlViewWidget(ui->centralWidget);
    vw->setGeometry(0,0,640,480);

    vw->updateGL();
}

MainWindow::~MainWindow()
{
    delete ui;
}
