#include <QApplication>
#include "mainwindow.h"
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;

    MainWindow mainwindow;

    bool res = translator.load("didftp_ko");
    qDebug()<<res;
    a.installTranslator(&translator);
    //mainwindow.show();

    return a.exec();
}

//TODO : how to communicate another Widget or window or dialog
//TODO : FTP functions (connect, put, get, delete, mkdir, list)
//TODO : monitoring functions
//TODO : make playlist.txt
