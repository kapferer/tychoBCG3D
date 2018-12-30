#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "glwidget.h"
#include "help.h"

#include <math.h>
#include <stdio.h>
#include <QFileDialog>
#include <QVector>
#include <QDebug>
#include <QColorDialog>
#include <QMessageBox>

/**
 * This is the MainWindow with the logic for tychoBCG3D,
 * TYCHO's 3D-boundary condition generator for
 * STL- and point data.
 *
 * Basically a boundary is mapped on a regular 3D grid.
 * This regular grid can be stored as a TYCHO boundary file,
 * ready to use in a TYCHO simulation.
 *
 * @brief MainWindow::MainWindow
 * @param parent
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    ui->progressBar->setHidden(true);

    counter = 0;
    norm_faktor = 2.0;

    //set the default colors
    ui->glwidget->redAmbient = 0.92f;
    ui->glwidget->greenAmbient = 0.58f;
    ui->glwidget->blueAmbient = 0.18f;

    ui->glwidget->redModel = 0.5;
    ui->glwidget->greenModel = 0.5;
    ui->glwidget->blueModel = 0.5;

    ui->glwidget->backroundColor.setAlpha(1.0);
    ui->glwidget->backroundColor.setRedF(0.2);
    ui->glwidget->backroundColor.setGreenF(0.2);
    ui->glwidget->backroundColor.setBlueF(0.2);

    ui->glwidget->drawer = 1;

    ui->glwidget->shininess = 50;
    ui->glwidget->point_size = 1;

    //menu
    connect(ui->actionOpen_Point_Data, SIGNAL(triggered()), this, SLOT(openfile_point_data()));
    connect(ui->actionOpen_STL_Data_2, SIGNAL(triggered()), this, SLOT(openfile_stl_data()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(on_actionQuit_triggered()));
    connect(ui->actionReset_Boundary, SIGNAL(triggered()), this, SLOT(on_actionReset_Bounding_Box_triggered()));
    connect(ui->actionMake_Boundary, SIGNAL(triggered()), this, SLOT(on_actionMake_Boundary_Conditions_triggered()));
    connect(ui->actionSolid_Color, SIGNAL(triggered()), this, SLOT(modelColor()));
    connect(ui->actionAmbient_Color, SIGNAL(triggered()), this, SLOT(ambientColor()));
    connect(ui->actionBackground_Color, SIGNAL(triggered()), this, SLOT(setbackroundColor()));
    connect(ui->actionCalcualte_Normal_Vectors, SIGNAL(triggered()), this, SLOT(calculateNormalVector()));
    connect(ui->actionHelp_2, SIGNAL(triggered()), this, SLOT(showHelp()));

    //menubar
    connect(ui->actionOpen_File_point_data, SIGNAL(triggered()), this, SLOT(openfile_point_data()));
    connect(ui->actionOpen_STL_Data, SIGNAL(triggered()), this, SLOT(openfile_stl_data()));
    connect(ui->actionColor_ambient_light, SIGNAL(triggered()), this, SLOT(ambientColor()));
    connect(ui->actionColor_model, SIGNAL(triggered()), this, SLOT(modelColor()));
    connect(ui->actionCalculate_normal_vectors, SIGNAL(triggered()), this, SLOT(calculateNormalVector()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(on_actionQuit_triggered()));
    connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(showHelp()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));

    //controls
    connect(ui->edit_xmin, SIGNAL(valueChanged(double)), this, SLOT(on_resize_bounding_box_text_lines()));
    connect(ui->edit_ymin, SIGNAL(valueChanged(double)), this, SLOT(on_resize_bounding_box_text_lines()));
    connect(ui->edit_zmin, SIGNAL(valueChanged(double)), this, SLOT(on_resize_bounding_box_text_lines()));
    connect(ui->edit_xmax, SIGNAL(valueChanged(double)), this, SLOT(on_resize_bounding_box_text_lines()));
    connect(ui->edit_ymax, SIGNAL(valueChanged(double)), this, SLOT(on_resize_bounding_box_text_lines()));
    connect(ui->edit_zmax, SIGNAL(valueChanged(double)), this, SLOT(on_resize_bounding_box_text_lines()));
    connect(ui->res_x,SIGNAL(editingFinished()),this, SLOT(alter_resolution_x()));
    connect(ui->res_y,SIGNAL(editingFinished()),this, SLOT(alter_resolution_y()));
    connect(ui->res_z,SIGNAL(editingFinished()),this, SLOT(alter_resolution_z()));
    connect(ui->drawer_chooser, SIGNAL(currentIndexChanged(QString)), this, SLOT(set_drawer()));
    connect(ui->dial, SIGNAL(sliderMoved(int)), this, SLOT(setShininess()));
    connect(ui->dial_point_size, SIGNAL(sliderMoved(int)), this, SLOT(changePointSize()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * Open point data and shifts the coordiante system to the centre and norms it to norm_faktor.
 *
 * @brief MainWindow::openfile_point_data
 */
void MainWindow::openfile_point_data()
{
    FILE *fd;
    char tmp_buffer[100];
    float  x, y, z, vx, vy, vz;
    int res_x, res_y, res_z;
    int counter = 0;
    int triangular_facets = 0;

    ui->glwidget->stl_data[0].clear();
    ui->glwidget->stl_data[1].clear();
    ui->glwidget->stl_data[2].clear();

    ui->glwidget->coordiantes[0].clear();
    ui->glwidget->coordiantes[1].clear();
    ui->glwidget->coordiantes[2].clear();
    ui->glwidget->coordiantes[3].clear();
    ui->glwidget->coordiantes[4].clear();
    ui->glwidget->coordiantes[5].clear();

    ui->glwidget->sampled_data[0].clear();
    ui->glwidget->sampled_data[1].clear();
    ui->glwidget->sampled_data[2].clear();

    ui->glwidget->x_min = ui->glwidget->x_max = ui->glwidget->y_min = ui->glwidget->y_max = ui->glwidget->z_min = ui->glwidget->z_max = 0.0;
    x_min_init = y_min_init = z_min_init = x_max_init = y_max_init = z_max_init = 0.0;

    file = QFileDialog::getOpenFileName(this, tr("Open Point-Data File"),"",tr("FILE (*.txt)"));

    //Now we read the file
    ui->glwidget->coordiantes[0].clear();
    ui->glwidget->coordiantes[1].clear();
    ui->glwidget->coordiantes[2].clear();

    if (file.length() != 0){

        fd = fopen(file.toUtf8().constData(),"r");

        ui->progressBar->setHidden(false);
        ui->progressBar->setValue(0);

        fscanf(fd,"%f %f %f\n", &x_min_init, &y_min_init, &z_min_init);

        x_max_init = x_min_init;
        y_max_init = y_min_init;
        z_max_init = z_min_init;

        fseek(fd,0,0);

        while(!feof(fd)){
            fscanf(fd,"%s", &tmp_buffer);
            if (strcmp(tmp_buffer,"\n") != 0) counter++;

        }

        triangular_facets = counter/6;

        counter = 0;

        fseek(fd,0,0);

        while(!feof(fd)){

            fscanf(fd,"%f %f %f %f %f %f\n", &x, &y, &z, &vx, &vy, &vz);

            ui->glwidget->coordiantes[0].append(x);
            ui->glwidget->coordiantes[1].append(y);
            ui->glwidget->coordiantes[2].append(z);

            ui->glwidget->coordiantes[3].append(vx);
            ui->glwidget->coordiantes[4].append(vy);
            ui->glwidget->coordiantes[5].append(vz);

            if (x < x_min_init) x_min_init = x;
            if (x > x_max_init) x_max_init = x;
            if (y < y_min_init) y_min_init = y;
            if (y > y_max_init) y_max_init = y;
            if (z < z_min_init) z_min_init = z;
            if (z > z_max_init) z_max_init = z;

            counter++;

            ui->progressBar->setValue(100.0/triangular_facets*counter);

        }

        ui->progressBar->setHidden(true);

        fclose(fd);

        diameter_x = x_max_init - x_min_init;
        diameter_y = y_max_init - y_min_init;
        diameter_z = z_max_init - z_min_init;

        for (int i = 0; i < ui->glwidget->coordiantes[0].size(); i++){

            ui->glwidget->coordiantes[0][i] = (ui->glwidget->coordiantes[0][i] - x_min_init - diameter_x/2.0) * (norm_faktor / diameter_x);
            ui->glwidget->coordiantes[1][i] = (ui->glwidget->coordiantes[1][i] - y_min_init - diameter_y/2.0) * (norm_faktor / diameter_x);
            ui->glwidget->coordiantes[2][i] = (ui->glwidget->coordiantes[2][i] - z_min_init - diameter_z/2.0) * (norm_faktor / diameter_x);
        }

        ui->glwidget->x_min = x_min_init = (-diameter_x/2.0)*(norm_faktor / diameter_x);
        ui->glwidget->y_min = y_min_init = (-diameter_y/2.0)*(norm_faktor / diameter_x);
        ui->glwidget->z_min = z_min_init = (-diameter_z/2.0)*(norm_faktor / diameter_x);

        ui->glwidget->x_max = x_max_init = (diameter_x/2.0)*(norm_faktor / diameter_x);
        ui->glwidget->y_max = y_max_init = (diameter_y/2.0)*(norm_faktor / diameter_x);
        ui->glwidget->z_max = z_max_init = (diameter_z/2.0)*(norm_faktor / diameter_x);

        res_x = (int)(300.0);
        res_y = (int)((300.0/diameter_x * diameter_y) + 0.5);
        res_z = (int)((300.0/diameter_x * diameter_z) + 0.5);;

        ui->res_x->setValue(res_x);
        ui->res_y->setValue(res_y);
        ui->res_z->setValue(res_z);

        ui->edit_xmin->setValue(x_min_init);
        ui->edit_ymin->setValue(y_min_init);
        ui->edit_zmin->setValue(z_min_init);
        ui->edit_xmax->setValue(x_max_init);
        ui->edit_ymax->setValue(y_max_init);
        ui->edit_zmax->setValue(z_max_init);


        ui->glwidget->updateGL();
    }
}

/**
 * Opens stl data and shifts the coordiante system to the centre and norms it to norm_faktor
 * @brief MainWindow::openfile_stl_data
 */
void MainWindow::openfile_stl_data()
{
    FILE *fd;
    char buffer[50];
    char tmp_buffer[100];
    int bin_or_string = 0;
    float  x, y, z;
    float norm_x, norm_y, norm_z;
    int res_x, res_y, res_z;
    unsigned int counter = 0;
    unsigned short attribute;
    unsigned int triangular_facets;

    ui->glwidget->stl_data[0].clear();
    ui->glwidget->stl_data[1].clear();
    ui->glwidget->stl_data[2].clear();

    ui->glwidget->coordiantes[0].clear();
    ui->glwidget->coordiantes[1].clear();
    ui->glwidget->coordiantes[2].clear();
    ui->glwidget->coordiantes[3].clear();
    ui->glwidget->coordiantes[4].clear();
    ui->glwidget->coordiantes[5].clear();

    ui->glwidget->sampled_data[0].clear();
    ui->glwidget->sampled_data[1].clear();
    ui->glwidget->sampled_data[2].clear();

    ui->glwidget->x_min = ui->glwidget->x_max = ui->glwidget->y_min = ui->glwidget->y_max = ui->glwidget->z_min = ui->glwidget->z_max = 0.0;
    x_min_init = y_min_init = z_min_init = x_max_init = y_max_init = z_max_init = 0.0;

    x = y = z = 0.0;
    //buffer .clear();

    file = QFileDialog::getOpenFileName(this, tr("Open STL-File"),"",tr("FILE (*.stl)"));

    //Now we read the file
    ui->glwidget->stl_data[0].clear();
    ui->glwidget->stl_data[1].clear();
    ui->glwidget->stl_data[2].clear();

    if (file.length() != 0){

        ui->progressBar->setHidden(false);
        ui->progressBar->setValue(0);

        fd = fopen(file.toUtf8().constData(),"r");

        //decide binary or text file
        fread(&buffer, 6, sizeof(char), fd);
        fseek(fd,0,0);

        QString tmp = "";

        for (int i = 0; i < 5; i++) tmp+=buffer[i];

        if (tmp == "solid") bin_or_string = 0;
        if (tmp != "solid") bin_or_string = 1;

        fclose(fd);

        counter = 0;

        qDebug() << bin_or_string;

        //an ascii stl file
        if (bin_or_string == 0){

            fd = fopen(file.toUtf8().constData(),"r");

            while(!feof(fd)){

                fscanf(fd,"%s", &tmp_buffer);
                if (strcmp(tmp_buffer,"normal") == 0) counter++;
                if (strcmp(tmp_buffer,"vertex") == 0) counter++;
            }

            triangular_facets = counter;
            counter = 0;

            fseek(fd,0,0);

            while(!feof(fd)){
                fscanf(fd,"%s", &tmp_buffer);

                if (strcmp(tmp_buffer,"normal") == 0){

                    fscanf(fd,"%f %f %f", &norm_x, &norm_y, &norm_z);

                    ui->glwidget->stl_data[0].append(norm_x);
                    ui->glwidget->stl_data[1].append(norm_y);
                    ui->glwidget->stl_data[2].append(norm_z);

                    counter++;
                }

                if (strcmp(tmp_buffer,"vertex") == 0){

                    fscanf(fd,"%f %f %f", &x, &y, &z);
                    counter++;

                    if (counter == 1){

                        x_max_init = x_min_init = x;
                        y_max_init = y_min_init = y;
                        z_max_init = z_min_init = z;
                    }

                    ui->glwidget->stl_data[0].append(x);
                    ui->glwidget->stl_data[1].append(y);
                    ui->glwidget->stl_data[2].append(z);

                    if (x < x_min_init) x_min_init = x;
                    if (x > x_max_init) x_max_init = x;
                    if (y < y_min_init) y_min_init = y;
                    if (y > y_max_init) y_max_init = y;
                    if (z < z_min_init) z_min_init = z;
                    if (z > z_max_init) z_max_init = z;

                    counter++;

                }

                ui->progressBar->setValue(100.0/triangular_facets*counter);
            }

            ui->progressBar->setHidden(true);

            fclose(fd);
        }

        counter = 0;

        //a binary stl file
        if (bin_or_string == 1){

            fd = fopen(file.toUtf8().constData(),"rb");

            //shift a couple of bits
            fseek(fd,80,0);

            fread(&triangular_facets, 1, sizeof(unsigned int), fd);

            while(counter < triangular_facets){

                norm_x = norm_y = norm_z = 0.0;
                x = y = z = 0.0;

                fread(&norm_x, 1, sizeof(float), fd);
                fread(&norm_y, 1, sizeof(float), fd);
                fread(&norm_z, 1, sizeof(float), fd);

                ui->glwidget->stl_data[0].append(norm_x);
                ui->glwidget->stl_data[1].append(norm_y);
                ui->glwidget->stl_data[2].append(norm_z);

                fread(&x, 1, sizeof(float), fd);
                fread(&y, 1, sizeof(float), fd);
                fread(&z, 1, sizeof(float), fd);

                ui->glwidget->stl_data[0].append(x);
                ui->glwidget->stl_data[1].append(y);
                ui->glwidget->stl_data[2].append(z);

                fread(&x, 1, sizeof(float), fd);
                fread(&y, 1, sizeof(float), fd);
                fread(&z, 1, sizeof(float), fd);

                ui->glwidget->stl_data[0].append(x);
                ui->glwidget->stl_data[1].append(y);
                ui->glwidget->stl_data[2].append(z);

                fread(&x, 1, sizeof(float), fd);
                fread(&y, 1, sizeof(float), fd);
                fread(&z, 1, sizeof(float), fd);

                ui->glwidget->stl_data[0].append(x);
                ui->glwidget->stl_data[1].append(y);
                ui->glwidget->stl_data[2].append(z);

                fread(&attribute, 1, sizeof(unsigned short), fd);

                counter++;

                ui->progressBar->setValue(100.0/triangular_facets*counter);

                if (counter == 1){

                    x_max_init = x_min_init = x;
                    y_max_init = y_min_init = y;
                    z_max_init = z_min_init = z;
                }

                if (x < x_min_init) x_min_init = x;
                if (x > x_max_init) x_max_init = x;
                if (y < y_min_init) y_min_init = y;
                if (y > y_max_init) y_max_init = y;
                if (z < z_min_init) z_min_init = z;
                if (z > z_max_init) z_max_init = z;

            }

            ui->progressBar->setHidden(true);

            fclose(fd);
        }

        diameter_x = x_max_init - x_min_init;
        diameter_y = y_max_init - y_min_init;
        diameter_z = z_max_init - z_min_init;

        for (int i = 0; i < ui->glwidget->stl_data[0].size(); i++){

            if (i%4==0) continue;

            ui->glwidget->stl_data[0][i] = (ui->glwidget->stl_data[0][i] - x_min_init - diameter_x/2.0) * (norm_faktor / diameter_x);
            ui->glwidget->stl_data[1][i] = (ui->glwidget->stl_data[1][i] - y_min_init - diameter_y/2.0) * (norm_faktor / diameter_x);
            ui->glwidget->stl_data[2][i] = (ui->glwidget->stl_data[2][i] - z_min_init - diameter_z/2.0) * (norm_faktor / diameter_x);
        }

        ui->glwidget->x_min = x_min_init = (-diameter_x/2.0)*(norm_faktor / diameter_x);
        ui->glwidget->y_min = y_min_init = (-diameter_y/2.0)*(norm_faktor / diameter_x);
        ui->glwidget->z_min = z_min_init = (-diameter_z/2.0)*(norm_faktor / diameter_x);

        ui->glwidget->x_max = x_max_init = (diameter_x/2.0)*(norm_faktor / diameter_x);
        ui->glwidget->y_max = y_max_init = (diameter_y/2.0)*(norm_faktor / diameter_x);
        ui->glwidget->z_max = z_max_init = (diameter_z/2.0)*(norm_faktor / diameter_x);

        res_x = (int)(300.0);
        res_y = (int)((300.0/diameter_x * diameter_y) + 0.5);
        res_z = (int)((300.0/diameter_x * diameter_z) + 0.5);;

        ui->res_x->setValue(res_x);
        ui->res_y->setValue(res_y);
        ui->res_z->setValue(res_z);

        ui->edit_xmin->setValue(x_min_init);
        ui->edit_ymin->setValue(y_min_init);
        ui->edit_zmin->setValue(z_min_init);
        ui->edit_xmax->setValue(x_max_init);
        ui->edit_ymax->setValue(y_max_init);
        ui->edit_zmax->setValue(z_max_init);

        ui->glwidget->init();
        ui->glwidget->updateGL();

    }

}

void MainWindow::on_actionQuit_triggered()
{
    qApp->exit();
}

/**
 *  resizes the bounding box values
 *  and initiates an update in the
 *  GL viewer
 *
 * @brief MainWindow::on_resize_bounding_box_text_lines
 */
void MainWindow::on_resize_bounding_box_text_lines(){

    ui->glwidget->x_min = ui->edit_xmin->value();
    ui->glwidget->y_min = ui->edit_ymin->value();
    ui->glwidget->z_min = ui->edit_zmin->value();
    ui->glwidget->x_max = ui->edit_xmax->value();
    ui->glwidget->y_max = ui->edit_ymax->value();
    ui->glwidget->z_max = ui->edit_zmax->value();

    alter_resolution_x();
    alter_resolution_y();
    alter_resolution_z();

    ui->glwidget->updateGL();
}

/**
 * Resets the bounding box to the starting values
 * @brief MainWindow::on_actionReset_Bounding_Box_triggered
 */
void MainWindow::on_actionReset_Bounding_Box_triggered()
{

    ui->glwidget->x_min = x_min_init;
    ui->glwidget->y_min = y_min_init;
    ui->glwidget->z_min = z_min_init;
    ui->glwidget->x_max = x_max_init;
    ui->glwidget->y_max = y_max_init;
    ui->glwidget->z_max = z_max_init;

    ui->edit_xmin->setValue(x_min_init);
    ui->edit_ymin->setValue(y_min_init);
    ui->edit_zmin->setValue(z_min_init);
    ui->edit_xmax->setValue(x_max_init);
    ui->edit_ymax->setValue(y_max_init);
    ui->edit_zmax->setValue(z_max_init);

    alter_resolution_x();
    alter_resolution_y();
    alter_resolution_z();

    ui->glwidget->updateGL();

}

/**
 * Samples the triangles or point data to the grid
 *
 * @brief MainWindow::on_actionMake_Boundary_Conditions_triggered
 */
void MainWindow::on_actionMake_Boundary_Conditions_triggered()
{

    if ((ui->glwidget->stl_data[0].size()==0)&&(ui->glwidget->coordiantes[0].size()==0)){

        QMessageBox msgBox;
        msgBox.setText("Please load first some data.");
        msgBox.exec();
    }

    if ((ui->glwidget->stl_data[0].size()!=0)||(ui->glwidget->coordiantes[0].size()!=0)){

        FILE *fd;
        QString file_save;
        file_save = QFileDialog::getSaveFileName(this, tr("TYCHO boundary-conditions"),"",tr("Tycho Boundary File (*.tyc)"));
        QFileInfo file(file_save);

        if (!file_save.isEmpty()){

            if(file.suffix().isEmpty()) file_save += ".tyc";

            float sx, sy, sz;
            int ZAX, ZAY, ZAZ;
            int i, j, k, l;
            int tmp;
            int counter;

            float vertex1[3];
            float vertex2[3];
            float vertex3[3];

            //upper limits and values for running indices for the sample algol for stl data
            int nu, nw;
            int max_u, max_w, max_uw;
            int max_ax, max_ay, max_az;
            int max_bx, max_by, max_bz;

            //triangle distance-vectors
            float ux, uy, uz;
            float wx, wy, wz;

            //point on triangle
            float px, py, pz;

            ZAX = ui->res_x->value();
            ZAY = ui->res_y->value();
            ZAZ = ui->res_z->value();


            ui->glwidget->x_min = ui->edit_xmin->value();
            ui->glwidget->y_min = ui->edit_ymin->value();
            ui->glwidget->z_min = ui->edit_zmin->value();
            ui->glwidget->x_max = ui->edit_xmax->value();
            ui->glwidget->y_max = ui->edit_ymax->value();
            ui->glwidget->z_max = ui->edit_zmax->value();

            diameter_x = ui->glwidget->x_max - ui->glwidget->x_min;
            diameter_y = ui->glwidget->y_max - ui->glwidget->y_min;
            diameter_z = ui->glwidget->z_max - ui->glwidget->z_min;

            sx = (diameter_x/(float)ZAX);
            sy = (diameter_y/(float)ZAY);
            sz = (diameter_z/(float)ZAZ);

            bounding = (int ***)calloc((size_t)ZAX, sizeof(int **));
            for (k = 0; k < ZAX; k++) {
                bounding[k] = (int **)calloc((size_t)ZAY, sizeof(int *));
                if ( !bounding[k] ) {
                    fprintf(stderr, "Could not allocate memory.\n");
                    exit(1);
                }
                for (j = 0; j < ZAY; j++) {
                    bounding[k][j] = (int *)calloc((size_t)ZAZ, sizeof(int));
                    if ( !bounding[k][j] ) {
                        fprintf(stderr, "Could not allocate memory.\n");
                        exit(1);
                    }
                }
            }

            //Point Data sampled on a grid
            if (ui->glwidget->coordiantes[0].size() > 0){

                ui->progressBar->setHidden(false);
                ui->progressBar->setValue(0);

                counter = 0;

                for (l = 0; l < ui->glwidget->coordiantes[0].size(); l++)   // Für jedes Partikel
                {

                    ui->glwidget->coordiantes[0][l] += -(ui->glwidget->x_min);
                    ui->glwidget->coordiantes[1][l] += -(ui->glwidget->y_min);
                    ui->glwidget->coordiantes[2][l] += -(ui->glwidget->z_min);

                    i = (int)(floor(((ui->glwidget->coordiantes[0][l])/sx)));//Mit floor werden alle Partikel einer Zelle zugeordnet.Es werden also alle Partikel im selben sx der selben Zelle zugeordnet
                    j = (int)(floor(((ui->glwidget->coordiantes[1][l])/sy)));//Der nächste Gitterpunkt wird bestimmt. Mit -175 in x und y Koordinatentransformation, um Würfel ins Zentrum zu setzen.
                    k = (int)(floor(((ui->glwidget->coordiantes[2][l])/sz)));

                    if ((i < 0)||(j < 0)||(k < 0)||(i > ZAX - 1)||(j > ZAY - 1)||(k > ZAZ - 1)) {

                        ui->glwidget->coordiantes[0][l] += ui->glwidget->x_min;
                        ui->glwidget->coordiantes[1][l] += ui->glwidget->y_min;
                        ui->glwidget->coordiantes[2][l] += ui->glwidget->z_min;

                        continue;
                    }

                    bounding[i][j][k] += 1;

                    ui->glwidget->coordiantes[0][l] += ui->glwidget->x_min;
                    ui->glwidget->coordiantes[1][l] += ui->glwidget->y_min;
                    ui->glwidget->coordiantes[2][l] += ui->glwidget->z_min;

                    counter++;

                    ui->progressBar->setValue(100.0/ui->glwidget->coordiantes[0].size()*counter);

                }

                ui->progressBar->setHidden(true);
            }


            //STL Data sampled on a grid
            if (ui->glwidget->stl_data[0].size() > 0){

                ui->progressBar->setHidden(false);
                ui->progressBar->setValue(0);

                counter = 0;

                // for each triangle (remember first the norm vector, then the 3 vertices
                for (l = 0; l <= ui->glwidget->stl_data[0].size() - 4; l = l + 4)
                {

                    //just to be sure
                    vertex1[0] = ui->glwidget->stl_data[0].at(l+1)-(ui->glwidget->x_min);
                    vertex1[1] = ui->glwidget->stl_data[1].at(l+1)-(ui->glwidget->y_min);
                    vertex1[2] = ui->glwidget->stl_data[2].at(l+1)-(ui->glwidget->z_min);
                    vertex2[0] = ui->glwidget->stl_data[0].at(l+2)-(ui->glwidget->x_min);
                    vertex2[1] = ui->glwidget->stl_data[1].at(l+2)-(ui->glwidget->y_min);
                    vertex2[2] = ui->glwidget->stl_data[2].at(l+2)-(ui->glwidget->z_min);
                    vertex3[0] = ui->glwidget->stl_data[0].at(l+3)-(ui->glwidget->x_min);
                    vertex3[1] = ui->glwidget->stl_data[1].at(l+3)-(ui->glwidget->y_min);
                    vertex3[2] = ui->glwidget->stl_data[2].at(l+3)-(ui->glwidget->z_min);

                    ux = vertex2[0] - vertex1[0];
                    uy = vertex2[1] - vertex1[1];
                    uz = vertex2[2] - vertex1[2];

                    wx = vertex3[0] - vertex1[0];
                    wy = vertex3[1] - vertex1[1];
                    wz = vertex3[2] - vertex1[2];

                    max_ax = ceil(fabs(ux/sx));
                    max_ay = ceil(fabs(uy/sy));
                    max_az = ceil(fabs(uz/sz));

                    max_u = max_ax + max_ay + max_az;

                    max_bx = ceil(fabs(wx/sx));
                    max_by = ceil(fabs(wy/sy));
                    max_bz = ceil(fabs(wz/sz));

                    max_w = max_bx + max_by + max_bz;
                    max_uw = max_u + max_w;

                    for (nu = 0; nu <= max_uw; nu++){
                        for (nw = 0; nw <= max_uw - nu; nw++){

                            px = vertex1[0] + ((1.0 * nu)/max_uw) * ux + ((1.0 * nw)/max_uw) * wx;
                            py = vertex1[1] + ((1.0 * nu)/max_uw) * uy + ((1.0 * nw)/max_uw) * wy;
                            pz = vertex1[2] + ((1.0 * nu)/max_uw) * uz + ((1.0 * nw)/max_uw) * wz;

                            i = (int)(floor((px/sx)));
                            j = (int)(floor((py/sy)));
                            k = (int)(floor((pz/sz)));

                            if ((i < 0)||(j < 0)||(k < 0)||(i > ZAX - 1)||(j > ZAY - 1)||(k > ZAZ - 1)) continue;

                            bounding[i][j][k] = 1;
                        }
                    }

                    counter = counter + 4;

                    ui->progressBar->setValue(100.0/ui->glwidget->stl_data[0].size()*counter);
                }

                ui->progressBar->setHidden(true);
            }

            //here the TYCHO boundary file is written
            fd = fopen(file_save.toUtf8(), "w");
            fprintf(fd,"This is a TYCHO boundary condition file.\n");
            fprintf(fd,"X %i\n", ZAX);
            fprintf(fd,"Y %i\n", ZAY);
            fprintf(fd,"Z %i\n", ZAZ);
            fseek(fd, 200, SEEK_SET);

            for (int i = 0; i < ZAX; i++) {
                for (int j = 0; j < ZAY; j++) {
                    for (int k = 0; k < ZAZ; k++) {

                        tmp = 0;
                        if (bounding[i][j][k] > 0) tmp = 1;
                        fwrite(&tmp, 1, sizeof (int), fd);
                    }
                }
            }

            draw_sampled_data(ZAX, ZAY, ZAZ, bounding);

            free(bounding);

            fclose(fd);

        }
    }
}

/**
 * This routine assists to set up a cubish
 * regular grid
 *
 * @brief MainWindow::alter_resolution_x
 */
void MainWindow::alter_resolution_x(){

    int res_x;
    int res_y;
    int res_z;
    double dia_x;
    double dia_y;
    double dia_z;

    dia_x = ui->edit_xmax->value()-ui->edit_xmin->value();
    dia_y = ui->edit_ymax->value()-ui->edit_ymin->value();
    dia_z = ui->edit_zmax->value()-ui->edit_zmin->value();

    res_x = ui->res_x->value();

    if (dia_x != 0.0){
        res_y = (int)(res_x * dia_y / dia_x);
        ui->res_y->setValue(res_y);
    }
    if (dia_x != 0.0){
        res_z = (int)(res_x * dia_z / dia_x);
        ui->res_z->setValue(res_z);
    }
}

/**
 * This routine assists to set up a cubish
 * regular grid
 *
 * @brief MainWindow::alter_resolution_y
 */
void MainWindow::alter_resolution_y(){

    int res_x;
    int res_y;
    int res_z;
    double dia_x;
    double dia_y;
    double dia_z;

    dia_x = ui->edit_xmax->value()-ui->edit_xmin->value();
    dia_y = ui->edit_ymax->value()-ui->edit_ymin->value();
    dia_z = ui->edit_zmax->value()-ui->edit_zmin->value();

    res_y = ui->res_y->value();

    if (dia_y != 0.0){
        res_x = (int)(res_y * dia_x / dia_y);
        ui->res_x->setValue(res_x);
    }
    if (dia_y != 0.0){
        res_z = (int)(res_y * dia_z / dia_y);
        ui->res_z->setValue(res_z);
    }
}

/**
 * This routine assists to set up a cubish
 * regular grid
 *
 * @brief MainWindow::alter_resolution_z
 */
void MainWindow::alter_resolution_z(){

    int res_x;
    int res_y;
    int res_z;
    double dia_x;
    double dia_y;
    double dia_z;

    dia_x = ui->edit_xmax->value()-ui->edit_xmin->value();
    dia_y = ui->edit_ymax->value()-ui->edit_ymin->value();
    dia_z = ui->edit_zmax->value()-ui->edit_zmin->value();

    res_z = ui->res_z->value();

    if (dia_z != 0.0){
        res_x = (int)(res_z * dia_x / dia_z);
        ui->res_x->setValue(res_x);
    }
    if (dia_z != 0.0){
        res_y = (int)(res_z * dia_y / dia_z);
        ui->res_y->setValue(res_y);
    }
}

/**
 * Changes the ambient color in the GL Viewer
 *
 * @brief MainWindow::ambientColor
 */
void MainWindow::ambientColor()
{

    QColor color = QColorDialog::getColor(Qt::black, this, "Ambient Colot", QColorDialog::DontUseNativeDialog);
    if(color.isValid())
    {

        ui->glwidget->redAmbient = (float)color.red()/255.0;
        ui->glwidget->greenAmbient = (float)color.green()/255.0;
        ui->glwidget->blueAmbient = (float)color.blue()/255.0;

        ui->glwidget->init();
        ui->glwidget->updateGL();
    }
}

/**
 * Changes the model color in the GL Viewer
 * @brief MainWindow::modelColor
 */
void MainWindow::modelColor()
{

    QColor color = QColorDialog::getColor(Qt::black, this, "Model Color", QColorDialog::DontUseNativeDialog);
    if(color.isValid())
    {

        ui->glwidget->redModel = (float)color.red()/255.0;
        ui->glwidget->greenModel = (float)color.green()/255.0;
        ui->glwidget->blueModel = (float)color.blue()/255.0;

        ui->glwidget->init();
        ui->glwidget->updateGL();
    }
}

/**
 * Changes the backroung color in the GL Viewer
 * @brief MainWindow::setbackroundColor
 */
void MainWindow::setbackroundColor()
{

    QColor color = QColorDialog::getColor(Qt::black, this, "Baclground Color", QColorDialog::DontUseNativeDialog);
    if(color.isValid())
    {

        ui->glwidget->backroundColor = color;

        ui->glwidget->init();
        ui->glwidget->updateGL();
    }
}

/**
 * Calculates a normal vector for STL data
 *
 * @brief MainWindow::calculateNormalVector
 */
void MainWindow::calculateNormalVector(){

    int l;

    float vertex1[3];
    float vertex2[3];
    float vertex3[3];

    float ux;
    float uy;
    float uz;

    float wx;
    float wy;
    float wz;

    float xComp;
    float yComp;
    float zComp;

    if (ui->glwidget->stl_data[0].size()>0){


        for (l = 0; l <= ui->glwidget->stl_data[0].size() - 4; l = l + 4)
        {

            //just to be sure
            vertex1[0] = ui->glwidget->stl_data[0].at(l+1)-(ui->glwidget->x_min);
            vertex1[1] = ui->glwidget->stl_data[1].at(l+1)-(ui->glwidget->y_min);
            vertex1[2] = ui->glwidget->stl_data[2].at(l+1)-(ui->glwidget->z_min);
            vertex2[0] = ui->glwidget->stl_data[0].at(l+2)-(ui->glwidget->x_min);
            vertex2[1] = ui->glwidget->stl_data[1].at(l+2)-(ui->glwidget->y_min);
            vertex2[2] = ui->glwidget->stl_data[2].at(l+2)-(ui->glwidget->z_min);
            vertex3[0] = ui->glwidget->stl_data[0].at(l+3)-(ui->glwidget->x_min);
            vertex3[1] = ui->glwidget->stl_data[1].at(l+3)-(ui->glwidget->y_min);
            vertex3[2] = ui->glwidget->stl_data[2].at(l+3)-(ui->glwidget->z_min);

            ux = vertex2[0] - vertex1[0];
            uy = vertex2[1] - vertex1[1];
            uz = vertex2[2] - vertex1[2];

            wx = vertex3[0] - vertex1[0];
            wy = vertex3[1] - vertex1[1];
            wz = vertex3[2] - vertex1[2];

            xComp = uy*wz - uz*wy;
            yComp = uz*wx - ux*wz;
            zComp = ux*wy - uy*wx;

            ui->glwidget->stl_data[0][l] = xComp;
            ui->glwidget->stl_data[1][l] = yComp;
            ui->glwidget->stl_data[2][l] = zComp;
        }
    }

    ui->glwidget->updateGL();

}

/**
 * Set-up the data for drwaing the
 * sampled boundary condition
 *
 * @brief MainWindow::draw_sampled_data
 * @param ZAX
 * @param ZAY
 * @param ZAZ
 * @param bounding
 */
void MainWindow::draw_sampled_data(int ZAX, int ZAY, int ZAZ, int ***bounding){

    ui->glwidget->x_min = ui->edit_xmin->value();
    ui->glwidget->y_min = ui->edit_ymin->value();
    ui->glwidget->z_min = ui->edit_zmin->value();
    ui->glwidget->x_max = ui->edit_xmax->value();
    ui->glwidget->y_max = ui->edit_ymax->value();
    ui->glwidget->z_max = ui->edit_zmax->value();

    ui->glwidget->sampled_data[0].clear();
    ui->glwidget->sampled_data[1].clear();
    ui->glwidget->sampled_data[2].clear();

    float spacing_x = (ui->glwidget->x_max - ui->glwidget->x_min)/(float)ZAX;
    float spacing_y = (ui->glwidget->y_max - ui->glwidget->y_min)/(float)ZAY;
    float spacing_z = (ui->glwidget->z_max - ui->glwidget->z_min)/(float)ZAZ;

    ui->progressBar->setHidden(false);

    for (int l = 0; l < ZAX; l++){
        for (int m = 0; m < ZAY; m++){
            for (int n = 0; n < ZAZ; n++){

                if (bounding[l][m][n] != 0){

                    ui->glwidget->sampled_data[0].append(ui->glwidget->x_min + l*spacing_x);
                    ui->glwidget->sampled_data[1].append(ui->glwidget->y_min + m*spacing_y);
                    ui->glwidget->sampled_data[2].append(ui->glwidget->z_min + n*spacing_z);

                }
            }
        }
        ui->progressBar->setValue(100.0/ZAX*l);
    }

    ui->progressBar->setHidden(true);

}

/**
 * To choose what should be drawn (the STL/Point- or sampled data)
 * @brief MainWindow::set_drawer
 */
void MainWindow::set_drawer(){

    if (ui->drawer_chooser->currentText().compare("Raw Data")==0) ui->glwidget->drawer = 1;
    if (ui->drawer_chooser->currentText().compare("Sampled Data")==0) ui->glwidget->drawer = 2;

    ui->glwidget->updateGL();
}

/**
 * Sets the point size with tie dial on the main window
 * @brief MainWindow::changePointSize
 */
void MainWindow::changePointSize(){

    ui->glwidget->point_size = ui->dial_point_size->value();
    ui->glwidget->updateGL();

}

/**
 * Sets the shininess with the dial on the main window
 * @brief MainWindow::setShininess
 */
void MainWindow::setShininess(){

    ui->glwidget->shininess = 128 - ui->dial->value();
    ui->glwidget->updateGL();
}

/**
 * Show the help
 * @brief MainWindow::showHelp
 */
void MainWindow::showHelp(){

    myhelp = new help;
    myhelp->setWindowModality(Qt::ApplicationModal);
    myhelp->setWindowFlags(Qt::WindowStaysOnTopHint);
    myhelp->show();

}

/**
 * Show the about
 * @brief MainWindow::showAbout
 */
void MainWindow::showAbout(){

    myAbout = new about;
    myAbout->setWindowModality(Qt::ApplicationModal);
    myAbout->setWindowFlags(Qt::WindowStaysOnTopHint);
    myAbout->show();

}
