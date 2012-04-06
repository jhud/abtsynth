#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class GlViewWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_actionSelect_ends_triggered(bool checked);

private:
    Ui::MainWindow *ui;
    GlViewWidget *mViewWidget;

};

#endif // MAINWINDOW_H
