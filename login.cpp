#include "login.h"
#include "ui_login.h"

#include "mainwindow.h"
#include "qftp.h"

#include <QDataStream>
#include <QFile>

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    ui->pwdText->setEchoMode(QLineEdit::Password);

    readInfo();

    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(connectFtp()));
    connect(this, SIGNAL(sendConnectInfo(QString,QString,QString)), parentWidget(), SLOT(connectFtp(QString,QString,QString)));
}

void Login::connectFtp()
{
    emit sendConnectInfo(ui->ipText->text(), ui->idText->text(), ui->pwdText->text());
}

void Login::readInfo()
{
    QFile file("temp.dat"); //FIXME
    if(!file.open(QFile::ReadOnly| QFile::Text)) {
        qDebug()<<"cannot read file";
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_3);

    QString ip, id, pwd;

    in >> ip >> id >> pwd;
    ui->ipText->insert(ip);
    ui->idText->insert(id);
    ui->pwdText->insert(pwd);

    file.close();
}

void Login::writeInfo()
{
    QFile file("temp.dat"); //FIXME
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        qDebug()<<"cannot write file";
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_3);

    out << ui->ipText->text() << ui->idText->text() << ui->pwdText->text();

    file.close();
    //TODO Check Error
}

Login::~Login()
{
    delete ui;
}

