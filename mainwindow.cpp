#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "monitor.h"
#include "upload.h"
#include "login.h"

#include <QMessageBox>
#include <QThread>

QStringList unparsedDirectory;
QString currentDirectory("");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createMenus();
    createTable();
    updateStatusBar();

    login = new Login(this);
    monitor = new Monitor(this);
    upload = new Upload(this);
    ftp = new QFtp();

    login->show();

    connect(ui->ftpList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(showUpload(QModelIndex)));

    row = 0;
}

MainWindow::~MainWindow()
{
    delete ui;

    delete login;
    delete monitor;
    delete upload;
    delete ftp;
}

void MainWindow::createMenus()
{
    controlMenu = menuBar()->addMenu(tr("&Control"));
    controlMenu->addAction(refreshAction);
    controlMenu->addAction(controlAction);
    controlMenu->addSeparator();
    controlMenu->addAction(exitAction);
}

void MainWindow::createActions()
{
    refreshAction = new QAction(tr("&Refresh"), this);
    controlAction = new QAction(tr("&Control Monitor"), this);
    connect(controlAction, SIGNAL(triggered()), this, SLOT(showMonitor()));

    exitAction = new QAction(tr("&Exit"), this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::updateStatusBar()
{
    ui->statusbar->showMessage("status here");
}

void MainWindow::showMonitor()
{
    monitor->show();
}

void MainWindow::showUpload(QModelIndex index)
{
    qDebug()<<index;

    //check ftp directory

    QTableWidgetItem *item = ui->ftpList->item(index.row(),index.column());
    if(item && !item->text().isEmpty())
    {
        upload->initData(item->text());
        upload->show();
    }
}

void MainWindow::createTable()
{
    ui->ftpList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ftpList->setColumnCount(4);
    ui->ftpList->setRowCount(40);
    ui->ftpList->setHorizontalHeaderLabels(QStringList() << tr("Directory") << tr("Download") << tr("File") << tr("Subtitle"));
    ui->ftpList->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->ftpList->horizontalHeader()->setStretchLastSection(true);
   // ui->ftpList->verticalHeader()->setStretchLastSection(true);

    connect(ui->ftpList, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(insideList(QTableWidgetItem*)));
}

void MainWindow::insideList(QTableWidgetItem *item)
{
    //FIXME : get files recrusive
    //ftp->list(item->text());
}

void MainWindow::addToList(QUrlInfo url)
{
    qDebug()<<url.name();

    //TODO : check playlist.txt

#if 1
    if(url.isDir() && !url.isSymLink())
    {
        if(currentDirectory.isEmpty()) {
            unparsedDirectory << url.name();
            qDebug()<<"DIR NAME = "  + url.name();
        }
        else {
            unparsedDirectory << currentDirectory + "/" + url.name();
            qDebug()<<"DIR NAME = " + currentDirectory + "/" + url.name();
        }

#if 0
        QTableWidgetItem *item = new QTableWidgetItem(url.name());
        ui->ftpList->setItem(row,0,item);
        row++;
#endif
    }
#endif
}


void MainWindow::connectFtp(QString ip, QString id, QString pwd)
{
    //write info
    login->writeInfo();

    connect(ftp, SIGNAL(listInfo(QUrlInfo)),
            this, SLOT(addToList(QUrlInfo)));
    connect(ftp, SIGNAL(commandFinished(int,bool)),
            this, SLOT(ftpCommandFinished(int,bool)));
    connect(ftp, SIGNAL(dataTransferProgress(qint64,qint64)),
            this, SLOT(updateDataTransferProgress(qint64,qint64)));

    ftp->connectToHost(ip, 21); //FIXME : port editable
    ftp->login(id, pwd);

#if 0
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

}

void MainWindow::ftpCommandFinished(int, bool error)
{
    if (ftp->currentCommand() == QFtp::ConnectToHost) {
        if (error) {
            QMessageBox::information(this, tr("FTP"),
                                     tr("Unable to connect to the FTP server "
                                        "at %1. Please check that the host "
                                        "name is correct."));
            //connectOrDisconnect();
            return;
        }

        qDebug()<<"Connected..";
        //connected

        return;
    }

    if (ftp->currentCommand() == QFtp::Login)
    {
        if(error) {
            QMessageBox::information(this, tr("FTP"),tr("Can't log in. Please check that the id or password is correct."));

            return;
        }

        ftp->list();
        currentDirectory = "";

        login->hide();
        show();

        return;
    }

    if (ftp->currentCommand() == QFtp::List) //Recrusive
    {
        qDebug()<<"LIST COMMAND FINISHED";
        if(unparsedDirectory.size())
        {
            qDebug() << "NEXT DIR " + unparsedDirectory.at(0);
            ftp->list(unparsedDirectory.at(0));
            currentDirectory = unparsedDirectory.at(0);
            unparsedDirectory.removeFirst();
        }
    }
#if 0
//![7]

//![8]
    if (ftp->currentCommand() == QFtp::Get) {
        if (error) {
        //    statusLabel->setText(tr("Canceled download of %1.")
         //                        .arg(file->fileName()));
            file->close();
            file->remove();
        } else {
          //  statusLabel->setText(tr("Downloaded %1 to current directory.")
           //                      .arg(file->fileName()));
            file->close();
        }
        delete file;
        enableDownloadButton();
        progressDialog->hide();
//![8]
//![9]
    } else if (ftp->currentCommand() == QFtp::List) {
        if (isDirectory.isEmpty()) {
            fileList->addTopLevelItem(new QTreeWidgetItem(QStringList() << tr("<empty>")));
            fileList->setEnabled(false);
        }
    }
//![9]
#endif
}

void MainWindow::updateDataTransferProgress(qint64 readBytes,
                                           qint64 totalBytes)
{
    qDebug()<<"updateDataTransferProgress";
}

void MainWindow::removeFiles(QStringList removeList)
{
    int i;
    qDebug()<<"removeFiles";

    for(i=0;i<removeList.size();i++)
        qDebug()<<removeList.at(i);
}


void MainWindow::uploadFiles(QStringList uploadList)
{
    qDebug()<<"uploadFiles";

    int i;
    for(i=0;i<uploadList.size();i++)
        qDebug()<<uploadList.at(i);
}
