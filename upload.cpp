#include "upload.h"
#include "ui_upload.h"

#include <QFileDialog>
#include <QDebug>

Upload::Upload(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Upload)
{
    ui->setupUi(this);

    //read playlist.txt and add to fileList

    ui->immediatleyButton->setChecked(true);

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addClicked()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    connect(ui->transmitButton, SIGNAL(clicked()), this, SLOT(transmitClicked()));
}

Upload::~Upload()
{
    delete ui;
}

void Upload::addClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select Upload File"), ".", tr("*"));

    if(!fileName.isEmpty())
    {
        qDebug()<<fileName;
        ui->fileList->addItem(fileName);
    }
}

void Upload::deleteClicked()
{
    //find list local or ftp
    qDebug()<<"deleteClicked";
    QListWidgetItem *selected = ui->fileList->currentItem();
    if(selected)
    {
        qDebug()<<selected->text();
        if(!selected->text().contains(":/")) //ftp file to remove
            removeList<<selected->text();

        delete selected;
    }
}

void Upload::transmitClicked()
{
    //remove file from removeList
    //upload file from fileList contains ":/"
}

void Upload::updateList()
{
}
