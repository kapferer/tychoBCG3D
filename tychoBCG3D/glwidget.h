#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLViewer/qglviewer.h>
#include <QVector>

class GLWidget : public QGLViewer
{
public :
#if QT_VERSION < 0x040000
    GLWidget(QWidget *parent, const char *name);
#else
    GLWidget(QWidget *parent);
#endif

    float redAmbient;
    float greenAmbient;
    float blueAmbient;

    float redModel;
    float greenModel;
    float blueModel;

    int shininess;

    float x_min, x_max, y_min, y_max, z_min, z_max;

    int drawer;

    QColor backroundColor;

    //for point data
    QVector<float> coordiantes[6];
    //for stl data
    QVector<float> stl_data[3];
    //for the display of the sampled data
    QVector<float> sampled_data[3];

    int point_size;

    virtual void draw();
    virtual void draw_border(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max);
    virtual void init();

protected :


};

#endif // GLWIDGET_H
