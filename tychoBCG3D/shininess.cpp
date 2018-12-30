#include "shininess.h"
#include "ui_shininess.h"

Shininess::Shininess(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Shininess)
{
    ui->setupUi(this);
}

Shininess::~Shininess()
{
    delete ui;
}
