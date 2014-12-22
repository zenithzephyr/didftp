#include "subtitle.h"
#include "ui_subtitle.h"
#include <QMessageBox>
#include <QTextStream>
#include <QTextCodec>

Subtitle::Subtitle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Subtitle)
{
    ui->setupUi(this);

    ui->immediatleyButton->setChecked(true);

    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->transmitButton, SIGNAL(clicked()), this, SLOT(transmitClicked()));
    connect(this, SIGNAL(uploadSubtitle(QString)), parentWidget(), SLOT(uploadSubtitle(QString)));
}

Subtitle::~Subtitle()
{
    delete ui;
}

void Subtitle::makeSubtitle()
{
    QFile file("subtitle.txt");
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
        QMessageBox::information(this, tr("FTP"),
                                 tr("Unable to write the file %1: %2.")
                                 .arg(file.fileName()).arg(file.errorString()));
    QTextStream out(&file);
    QDateTime now = QDateTime::currentDateTime();
    out.setCodec(QTextCodec::codecForName("eucKR"));

    out << "[ver:1]\n";
    out << "[send:" + QString::number(now.toTime_t()) + "]\n";
    out << ui->subtitleEdit->toPlainText();
    file.close();
}


void Subtitle::initData(QString subtext, QString path)
{
    ui->subtitleEdit->setPlainText(subtext);
    ui->dirName->setText(path);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit->setTime(QTime::currentTime());
}

void Subtitle::transmitClicked()
{
    if(!ui->subtitleEdit->toPlainText().isEmpty())
    {
        makeSubtitle();
        emit uploadSubtitle(ui->dirName->text());
    }
}

