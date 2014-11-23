#include "login.h"
#include "ui_login.h"

#include "mainwindow.h"
#include "qftp.h"

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    //get previous ip, name, pwd from files

    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(connectFtp()));
}

Login::~Login()
{
    delete ui;
}

void Login::addToList(QUrlInfo url)
{
    qDebug()<<url.name();
}

void Login::connectFtp()
{
   //try to connect ftp
    QFtp *ftp = new QFtp(this);

    connect(ftp, SIGNAL(listInfo(QUrlInfo)),
            this, SLOT(addToList(QUrlInfo)));

    ftp->connectToHost("192.168.0.112", 21);
    ftp->login("ftpuser","1234");

    ftp->list();
#if 0
    connect(ftp, SIGNAL(commandFinished(int,bool)),
            this, SLOT(ftpCommandFinished(int,bool)));
    connect(ftp, SIGNAL(dataTransferProgress(qint64,qint64)),
            this, SLOT(updateDataTransferProgress(qint64,qint64)));

    fileList->clear();
    currentPath.clear();
    isDirectory.clear();
//![1]

//![2]
    QUrl url(ftpServerLineEdit->text());
    if (!url.isValid() || url.scheme().toLower() != QLatin1String("ftp")) {
        ftp->connectToHost(ftpServerLineEdit->text(), 21);
        ftp->login();
    } else {
        ftp->connectToHost(url.host(), url.port(21));

        if (!url.userName().isEmpty())
            ftp->login(QUrl::fromPercentEncoding(url.userName().toLatin1()), url.password());
        else
            ftp->login();
        if (!url.path().isEmpty())
            ftp->cd(url.path());
    }
#endif

   //if connected goto mainwindow

   MainWindow *mainwindow = new MainWindow();
   //MainWindow mainwindow;
   mainwindow->show();

   hide();
   //quit();
}
