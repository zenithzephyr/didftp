#include "upload.h"
#include "ui_upload.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

Upload::Upload(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Upload)
{
    ui->setupUi(this);

    ui->immediatleyButton->setChecked(true);

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addClicked()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    connect(ui->transmitButton, SIGNAL(clicked()), this, SLOT(transmitClicked()));
    connect(this, SIGNAL(removeFiles(QString, QStringList)), parentWidget(), SLOT(removeFiles(QString, QStringList)));
    connect(this, SIGNAL(uploadFiles(QString, QStringList)), parentWidget(), SLOT(uploadFiles(QString, QStringList)));

    fileSizeMap.clear();
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
        if(!selected->text().contains(":/") && !selected->text().startsWith("/")) //ftp file to remove WINDOWS Only
            removeList<<selected->text();

        delete selected;
    }
}

void Upload::transmitClicked()
{
    int i;

    //remove file from removeList
    emit removeFiles(ui->directoryText->text(), removeList);
    //upload file from fileList contains ":/"
    for(i=0;i<ui->fileList->count();i++)
    {
        if(ui->fileList->item(i)->text().contains(":/") || ui->fileList->item(i)->text().startsWith("/"))
            uploadList<<ui->fileList->item(i)->text();
    }

    makePlaylist();

    uploadList << "playlist.txt";

    emit uploadFiles(ui->directoryText->text(), uploadList);

    //TODO change button text to "Cancel"
}

void Upload::makePlaylist()
{
    QFile file("playlist.txt");
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
        QMessageBox::information(this, tr("FTP"),
                                 tr("Unable to write the file %1: %2.")
                                 .arg(file.fileName()).arg(file.errorString()));
    QTextStream out(&file);
    QDateTime now = QDateTime::currentDateTime();

    out << "[ver:1]\n";
    out << "[send:" + QString::number(now.toTime_t()) + "]\n";
    int i;
    for(i=0;i<ui->fileList->count();i++)
    {
        QFileInfo info(ui->fileList->item(i)->text());
        QString fileSize;
        qDebug() << info.fileName();

        if(ui->fileList->item(i)->text().contains(":/") || ui->fileList->item(i)->text().startsWith("/")) //local files
        {
            fileSize = QString::number(info.size());
            qDebug() <<"SIZE : " << fileSize;
        }
        else //ftp file
        {
            fileSize = QString::number(fileSizeMap[ui->fileList->item(i)->text()]); //FIXME HERE
            qDebug() <<"SIZE : " << fileSize;
        }

        out << "[" + info.fileName() + "," + fileSize + "]\n";
    }
    out << "[end]\n";
    file.close();
}

void Upload::initData(QString directoryName, QStringList fileList, QMap<QString, qint64> sizeMap) //FIXME Later...
{
    ui->directoryText->setText(directoryName);
    removeList.clear();
    uploadList.clear();
    ui->fileList->clear();
    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit->setTime(QTime::currentTime());

    ui->progressBar->setValue(0);

    ui->fileList->addItems(fileList);

    fileSizeMap = sizeMap;
}

void Upload::setProgress(qint64 readBytes, qint64 totalBytes)
{
    ui->progressBar->setValue((readBytes*100/totalBytes));
    ui->progressBar->show();
}
