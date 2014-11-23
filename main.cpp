#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow mainwindow;

    //mainwindow.show();

    return a.exec();
}

//TODO : how to communicate another Widget or window or dialog
//TODO : FTP functions (connect, put, get, delete, mkdir, list)
//TODO : monitoring functions
//TODO : make playlist.txt
