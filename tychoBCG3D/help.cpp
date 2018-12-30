#include "help.h"
#include "ui_help.h"

help::help(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::help)
{
    ui->setupUi(this);

    ui->textBrowser->setOpenLinks(true);
    ui->textBrowser->setSource(QUrl(":docu/help.html"));
}

help::~help()
{
    delete ui;
}

void help::on_pushButton_clicked()
{
    close();
}
