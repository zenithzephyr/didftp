#include "login.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login login;

    login.show();

    return a.exec();
}

//TODO : how to communicate another Widget or window or dialog
//TODO : FTP functions (connect, put, get, delete, mkdir, list)
//TODO : monitoring functions
//TODO : make playlist.txt
