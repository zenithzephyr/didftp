#include "createdir.h"
#include "ui_createdir.h"

CreateDir::CreateDir(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateDir)
{
    ui->setupUi(this);
}

CreateDir::~CreateDir()
{
    delete ui;
}
