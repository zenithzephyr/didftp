#include <QApplication>
#include "mainwindow.h"
#include <QTranslator>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resource);

    QApplication a(argc, argv);
    QTranslator translator;

    bool res = translator.load("didftp_ko");
    qDebug()<<res;
    a.installTranslator(&translator);

    MainWindow mainwindow;

    return a.exec();
}

//TODO : monitoring functions
