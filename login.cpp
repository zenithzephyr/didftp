#include "login.h"
#include "ui_login.h"

#include "mainwindow.h"

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

void Login::connectFtp()
{
   //try to connect ftp

   //if connected goto mainwindow

   MainWindow *mainwindow = new MainWindow();
   //MainWindow mainwindow;
   mainwindow->show();

   hide();
   //quit();
}
