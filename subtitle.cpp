#include "subtitle.h"
#include "ui_subtitle.h"

Subtitle::Subtitle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Subtitle)
{
    ui->setupUi(this);
}

Subtitle::~Subtitle()
{
    delete ui;
}
