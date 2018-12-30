#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "help.h"
#include "about.h"

#include <QMainWindow>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString file;
    int ***bounding;
    int norm_faktor;
    float diameter_x;
    float diameter_y;
    float diameter_z;
    int counter;
    float x_min_init, x_max_init, y_min_init, y_max_init, z_min_init, z_max_init;

    help *myhelp;
    about *myAbout;

    void draw_border(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max);
    void draw_sampled_data(int ZAX, int ZAY, int ZAZ, int ***bounding);

private:
    Ui::MainWindow *ui;


private slots:
    void openfile_point_data();
    void openfile_stl_data();
    void alter_resolution_x();
    void alter_resolution_y();
    void alter_resolution_z();
    void on_actionQuit_triggered();
    void on_resize_bounding_box_text_lines();
    void on_actionReset_Bounding_Box_triggered();
    void on_actionMake_Boundary_Conditions_triggered();
    void on_actionMake_Soundemitter_triggered();
    void calculateNormalVector();
    void ambientColor();
    void modelColor();
    void setbackroundColor();
    void set_drawer();
    void setShininess();
    void showHelp();
    void showAbout();
    void changePointSize();

};

#endif // MAINWINDOW_H

