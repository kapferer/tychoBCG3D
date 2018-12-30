#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowModality(Qt::ApplicationModal);
    //w.setWindowModality(Qt::WindowModality);
    w.show();
    return a.exec();
}
