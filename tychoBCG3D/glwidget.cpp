/****************************************************************************

 Copyright (C) 2002-2011 Gilles Debunne. All rights reserved.

 This file is part of the QGLViewer library version 2.3.17.

 http://www.libqglviewer.com - contact@libqglviewer.com

 This file may be used under the terms of the GNU General Public License
 versions 2.0 or 3.0 as published by the Free Software Foundation and
 appearing in the LICENSE file included in the packaging of this file.
 In addition, as a special exception, Gilles Debunne gives you certain
 additional rights, described in the file GPL_EXCEPTION in this package.

 libQGLViewer uses dual licensing. Commercial/proprietary software must
 purchase a libQGLViewer Commercial License.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#include "glwidget.h"
#include <math.h>
#include <QDebug>

// Constructor must call the base class constructor.
#if QT_VERSION < 0x040000
GLWidget::GLWidget(QWidget *parent, const char *name)
    : QGLViewer(parent, name)
    #else
GLWidget::GLWidget(QWidget *parent)
    : QGLViewer(parent)
    #endif
{

}

/**
 * Sets basic stuff for the GL viewer
 *
 * @brief GLWidget::init
 */
void GLWidget::init()
{

    setSceneRadius(10.0);
    setFPSIsDisplayed(true);

    setBackgroundColor(backroundColor);

    //Add ambient light
    GLfloat ambientColor[] = {redAmbient, greenAmbient, blueAmbient}; //Color (0.2, 0.2, 0.2)
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    // Light setup
    glEnable(GL_LIGHTING); //Enable lighting
    glEnable(GL_NORMALIZE); //Have OpenGL automatically normalize our normals
    glShadeModel(GL_SMOOTH); //Enable smooth shading
}

/**
 * Draws the points, triangles and the smapled data
 * @brief GLWidget::draw
 */
void GLWidget::draw()
{

    //to draw point data
    if ((coordiantes[0].size() > 0)&&(drawer == 1)){

        glPointSize((float)point_size);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glAlphaFunc(GL_GREATER, 0.5);
        glEnable(GL_DEPTH_TEST);

        GLfloat white[] = {redAmbient, greenAmbient, blueAmbient, 1.0f};
        glMaterialfv(GL_FRONT, GL_SPECULAR, white);
        GLfloat shininess1[] = {shininess};
        glMaterialfv(GL_FRONT, GL_SHININESS, shininess1);

        glBegin(GL_POINTS);
        for (int i = 0; i < coordiantes[0].size(); i++){
            glColor3f(redModel,greenModel,blueModel);
            glNormal3f((GLfloat)coordiantes[3][i], (GLfloat)coordiantes[4][i],(GLfloat)coordiantes[5][i]);
            glVertex3f((GLfloat)coordiantes[0][i], (GLfloat)coordiantes[1][i],(GLfloat)coordiantes[2][i]);

        }
        glEnd();

        if (coordiantes[0].size() != 0) draw_border(x_min, x_max, y_min, y_max, z_min, z_max);

        glDisable(GL_DEPTH_TEST);
    }

    //to draw stl data
    if ((stl_data[0].size() > 0)&&(drawer == 1)){

        glEnable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES );

        GLfloat white[] = {redAmbient, greenAmbient, blueAmbient, 1.0f};
        glMaterialfv(GL_FRONT, GL_SPECULAR, white);
        GLfloat shininess1[] = {shininess};
        glMaterialfv(GL_FRONT, GL_SHININESS, shininess1);

        for (int i = 0; i < stl_data[0].size()-4; i = i+4){


            glBegin(GL_TRIANGLES);
            glNormal3f((GLfloat)stl_data[0][i], (GLfloat)stl_data[1][i], (GLfloat)stl_data[2][i]);
            glColor3f(redModel,greenModel,blueModel);
            glVertex3f((GLfloat)stl_data[0][i+1], (GLfloat)stl_data[1][i+1], (GLfloat)stl_data[2][i+1]);
            glVertex3f((GLfloat)stl_data[0][i+2], (GLfloat)stl_data[1][i+2], (GLfloat)stl_data[2][i+2]);
            glVertex3f((GLfloat)stl_data[0][i+3], (GLfloat)stl_data[1][i+3], (GLfloat)stl_data[2][i+3]);
            glEnd();
        }

        if (stl_data[0].size() != 0) draw_border(x_min, x_max, y_min, y_max, z_min, z_max);

        glDisable(GL_DEPTH_TEST);
    }


    //to draw the sampled data
    if ((sampled_data[0].size() > 0)&&(drawer == 2)){

        glPointSize((float)point_size);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glAlphaFunc(GL_GREATER, 0.5);
        glEnable(GL_DEPTH_TEST);

        GLfloat white[] = {redAmbient, greenAmbient, blueAmbient, 1.0f};
        glMaterialfv(GL_FRONT, GL_SPECULAR, white);
        GLfloat shininess1[] = {shininess};
        glMaterialfv(GL_FRONT, GL_SHININESS, shininess1);

        glBegin(GL_POINTS);
        for (int i = 0; i < sampled_data[0].size(); i++){
            glColor3f(redModel,greenModel,blueModel);
            glVertex3f((GLfloat)sampled_data[0][i], (GLfloat)sampled_data[1][i],(GLfloat)sampled_data[2][i]);

        }
        glEnd();

        if (sampled_data[0].size() != 0) draw_border(x_min, x_max, y_min, y_max, z_min, z_max);

        glDisable(GL_DEPTH_TEST);
    }

}


/**
 * Draws the bounding box, which represents the computational domain
 *
 * @brief GLWidget::draw_border
 * @param x_min
 * @param x_max
 * @param y_min
 * @param y_max
 * @param z_min
 * @param z_max
 */
void GLWidget::draw_border(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max){

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glLineWidth(1.0f);
    glColor3f(redModel,greenModel,blueModel);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
    glEnable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);

    glVertex3f(x_max, y_max, z_max);
    glVertex3f(x_min, y_max, z_max);
    glVertex3f(x_min, y_min, z_max);
    glVertex3f(x_max, y_min, z_max);

    glVertex3f(x_max, y_max, z_min);
    glVertex3f(x_min, y_max, z_min);
    glVertex3f(x_min, y_min, z_min);
    glVertex3f(x_max, y_min, z_min);

    glVertex3f(x_max, y_max, z_max);
    glVertex3f(x_max, y_min, z_max);
    glVertex3f(x_max, y_min, z_min);
    glVertex3f(x_max, y_max, z_min);

    glVertex3f(x_min, y_max, z_max);
    glVertex3f(x_min, y_min, z_max);
    glVertex3f(x_min, y_min, z_min);
    glVertex3f(x_min, y_max, z_min);

    glVertex3f(x_max, y_max, z_max);
    glVertex3f(x_min, y_max, z_max);
    glVertex3f(x_min, y_max, z_min);
    glVertex3f(x_max, y_max, z_min);

    glVertex3f(x_max, y_min, z_max);
    glVertex3f(x_min, y_min, z_max);
    glVertex3f(x_min, y_min, z_min);
    glVertex3f(x_max, y_min, z_min);

    glEnd();

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_DEPTH_TEST);
}
