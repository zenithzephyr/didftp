#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QStringList>
#include <QString>
#include <QMainWindow>
#include <QTableWidget>
#include <QDebug>
#include <QMenuBar>
#include <QFile>
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
    QFile *file;

    Login *login;
    Upload *upload;
    Monitor *monitor;

    int row;

    void refreshTable();
    void createActions();
    void createMenus();
    void createTable();
    void updateStatusBar();
    void makeTableData();
    void getPlaylist();
    void manipulateData(const QString &path, const QString &fileName);
    void doUpload();
    void doRemove();

private slots:
    void showMonitor();
    void showUpload(QModelIndex index);

    void addToList(QUrlInfo url);
    void connectFtp(QString ip, QString id, QString pwd);
    void updateDataTransferProgress(qint64 readBytes, qint64 totalBytes);
    void ftpCommandFinished(int, bool error);
    void removeFiles(QString path, QStringList removeList);
    void uploadFiles(QString path, QStringList uploadList);
};

#endif // MAINWINDOW_H
