#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// Macro to extract a param from the UI
#define PARAM(x) (MainWindow::get().param(x))

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    static MainWindow & get();

    void setStatusText(const QString & text);

private slots:
    void on_actionSelect_ends_triggered(bool checked);

    void on_actionFrontCamera_triggered();

    void on_actionSideCamera_triggered();

    void on_actionAboveCamera_triggered();

    void on_actionRenderSolid_triggered();

    void on_actionRenderStickman_triggered();

    void on_actionRender_triggered();

    void on_action_Save_as_triggered();

    void on_actionClamp_Depth_triggered(bool checked);

    void on_action_Load_triggered();

public:
    float param(const QString & name);

private:

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
