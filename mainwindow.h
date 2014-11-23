#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QDebug>
#include <QMenuBar>
#include "qftp.h"
#include "login.h"
#include "upload.h"
#include "monitor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QAction *refreshAction;
    QAction *controlAction;
    QAction *exitAction;

    QMenu *controlMenu;

    QFtp *ftp;
    Login *login;
    Upload *upload;
    Monitor *monitor;

    int row;

    void createActions();
    void createMenus();
    void createTable();
    void updateStatusBar();

private slots:
    void insideList(QTableWidgetItem*);
    void showMonitor();
    void showUpload(QModelIndex index);

    void addToList(QUrlInfo url);
    void connectFtp(QString ip, QString id, QString pwd);
    void updateDataTransferProgress(qint64 readBytes,
                                           qint64 totalBytes);
    void ftpCommandFinished(int, bool error);
};

#endif // MAINWINDOW_H
