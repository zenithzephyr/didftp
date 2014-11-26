#include "upload.h"
#include "ui_upload.h"

#include <QFileDialog>
#include <QDebug>

Upload::Upload(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Upload)
{
    ui->setupUi(this);

    ui->immediatleyButton->setChecked(true);

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addClicked()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    connect(ui->transmitButton, SIGNAL(clicked()), this, SLOT(transmitClicked()));
    connect(this, SIGNAL(removeFiles(QStringList)), parentWidget(), SLOT(removeFiles(QStringList)));
    connect(this, SIGNAL(uploadFiles(QStringList)), parentWidget(), SLOT(uploadFiles(QStringList)));
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
    int i;

    //remove file from removeList
    emit removeFiles(removeList);
    //upload file from fileList contains ":/"
    for(i=0;i<ui->fileList->count();i++)
    {
        if(ui->fileList->item(i)->text().contains(":/"))
            uploadList<<ui->fileList->item(i)->text();
    }

    emit uploadFiles(uploadList);

    //TODO change button text to "Cancel"
}

void Upload::initData(QString directoryName)
{
    ui->directoryText->setText(directoryName);
    removeList.clear();
    uploadList.clear();
    ui->fileList->clear();
    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit->setTime(QTime::currentTime());

    ui->progressBar->setValue(0);

    //TODO : init fileList from ftp
    //TODO : read playlist.txt and add to fileList
    //TODO : Upload progress
}
