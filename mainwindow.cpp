#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "monitor.h"
#include "upload.h"
#include "login.h"

#include <QMessageBox>
#include <QFileInfo>

QStringList unparsedDirectory;
QStringList fullFilesList;
QStringList playlistPath;
QString currentDirectory("");
QMap<QString, QUrlInfo> fullFilesMap;
QMap<QString, QString> TimeMap;
QMap<QString, QString> playlistMap;

int num = 0;

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
}

void MainWindow::addToList(QUrlInfo url)
{
    qDebug()<<url.name();

    if(url.isDir() && !url.isSymLink())
    {
        if(currentDirectory.isEmpty())
            unparsedDirectory << url.name();
        else
            unparsedDirectory << currentDirectory + "/" + url.name();
    }

    if(currentDirectory.isEmpty())
    {
        if(url.isDir() && !url.isSymLink())
        {
            fullFilesList << url.name() + "/";
            fullFilesMap[url.name() + "/"] = url;
        }
        else
        {
            if(url.name() == "playlist.txt")
                playlistPath << "/";

            fullFilesList << url.name();
            fullFilesMap[url.name()] = url;
        }
    }
    else
    {
        if(url.isDir() && !url.isSymLink())
        {
            fullFilesList << currentDirectory + "/" + url.name() + "/";
            fullFilesMap[currentDirectory + "/" + url.name() + "/"] = url;
        }
        else
        {
            if(url.name() == "playlist.txt")
                playlistPath << currentDirectory + "/";

            fullFilesList << currentDirectory + "/" + url.name();
            fullFilesMap[currentDirectory + "/" + url.name()] = url;
        }
    }
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
        //connected but not logined

        return;
    }

    if (ftp->currentCommand() == QFtp::Login)
    {
        if(error) {
            QMessageBox::information(this, tr("FTP"),tr("Can't log in. Please check that the id or password is correct."));

            return;
        }

        ftp->list(); //get all list
        currentDirectory = "";

        login->hide();
        show();

        return;
    }

    if (ftp->currentCommand() == QFtp::List) //Recrusive
    {
        if(unparsedDirectory.size())
        {
            ftp->list(unparsedDirectory.at(0));
            currentDirectory = unparsedDirectory.at(0);
            unparsedDirectory.removeFirst();
        }
        else
        {
            fullFilesList.sort();
#if 0
            int i;
            for(i=0;i<fullFilesList.size();i++)
                qDebug()<<"[ " + fullFilesList.at(i) + " ]";
#endif
            getPlaylist();
        }
    }

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
        //      enableDownloadButton();
        //     progressDialog->hide();

        //TODO Check if(playlist downloading?)
        getPlaylist();
    }
#if 0
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

void MainWindow::makeTableData()
{
    int i;
    QTableWidgetItem *item = new QTableWidgetItem("/"); //root
    ui->ftpList->setItem(0,0,item);

    for(i=0;i<fullFilesList.size();i++)
    {
        if(fullFilesList.at(i).endsWith("/")) //directory
        {
            QTableWidgetItem *item = new QTableWidgetItem("/"+fullFilesMap[fullFilesList.at(i)].name());

            ui->ftpList->setItem(i+1,0,item);
            if(!TimeMap[fullFilesList.at(i)].isEmpty())
            {
            QTableWidgetItem *item2 = new QTableWidgetItem(TimeMap[fullFilesList.at(i)]);
            ui->ftpList->setItem(i+1,1,item2);
            }
        }
        else //files
        {
            QTableWidgetItem *item = new QTableWidgetItem(fullFilesMap[fullFilesList.at(i)].name());
            ui->ftpList->setItem(i+1,2,item);
        }
        row++;
    }
}

void MainWindow::getPlaylist()
{
    num++; //FIXME

    if(playlistPath.size())
    {
        QString fileName = QString("playlist%1.txt").arg(num); //FIXME filename : filename must can found.
        file = new QFile(fileName);

        if (!file->open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("FTP"),
                                     tr("Unable to save the file %1: %2.")
                                     .arg(fileName).arg(file->errorString()));
            delete file;
        }

        ftp->get(playlistPath.at(0) + "/playlist.txt",file);

        playlistMap[playlistPath.at(0)] = fileName;
        playlistPath.removeFirst();
    }
    else
    {
        QMapIterator<QString, QString> i(playlistMap);
        while(i.hasNext()) {
            i.next();
            qDebug()<< i.key() << ": " << i.value() << endl;
            manipulateData(i.key(), i.value()); //key : ftp playlistPath, value : downloaded playlist.txt fileName
        }

        //check playlist.txt file list
        //if has playlist remove directory files and download playlist file
        //manipulateData (delete playlist.txt and other files if playlist.txt directory)
        makeTableData();
    }
}

void MainWindow::manipulateData(const QString &path, const QString &fileName)
{
    //parse playlist.txt
    QFile *pFile = new QFile(fileName);

    if (!pFile->open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("FTP"),
                                 tr("Unable to save the file %1: %2.")
                                .arg(fileName).arg(file->errorString()));
        delete file;
    }

    QTextStream in(pFile);
    QString line;
    QStringList playlistFiles;
#if 0
    QRegExp token1("[ver:");
    QRegExp token2("[send:");
    QRegExp token3("[run:");
    QRegExp token4("[end]");
#endif

    do {
        line = in.readLine();
      //  if(!(token1.indexIn(line) ||
       //         token2.indexIn(line) ||
        //        token3.indexIn(line) ||
         //       token4.indexIn(line))) //not match ver: end] send: run:
        if(line.startsWith("[ver:"))
        {
            //version parsing
        }
        else if(line.startsWith("[send:"))
        {
           //time parsing
            QStringList token = line.split(":");

            QString clist = token.at(1);

            QDateTime dtime;
            dtime.setTime_t(clist.remove(QChar(']')).toInt());
            TimeMap[path] =dtime.toString("yyyy/mm/dd hh:mm:ss");
        }
        else if(line.startsWith("[run:"))
        {
            //? not implemented
        }
        else if(line.startsWith("[end]"))
        {
            //end tag
        }
        else if(!line.isEmpty())
        {
            line.remove(QChar('['));
            //cut string
            QStringList clist = line.split(",");
            playlistFiles << clist.at(0);
            qDebug()<<"File : " << clist.at(0);
        }
    } while(!line.isNull());


    pFile->close();
    delete pFile;

    int i, j;
    for(i=0;i<fullFilesList.size();i++)
    {
        for(j=0;j<playlistFiles.size();j++)
    {
        if(fullFilesList.at(i).contains(playlistFiles.at(j)) && !fullFilesList.at(i).contains("///"))
        {
            QString addpath = fullFilesList.at(i) + "///";

            fullFilesList << addpath;

            fullFilesList.removeAt(i);
            i--;
        }
    }

        if(fullFilesList.at(i).startsWith(path) && (fullFilesList.at(i).count("/") == path.count("/")) && fullFilesList.at(i) != path)
        {
            //delete files
            qDebug() << "DELETED : " << fullFilesList.at(i);
            fullFilesMap.remove(fullFilesList.at(i));
            fullFilesList.removeAt(i);
            i--;
        }
    }

    for(i=0;i<fullFilesList.size();i++)
    {
        if(fullFilesList.at(i).contains("///"))
        {
        QString removepath = fullFilesList.at(i);
        removepath.replace("///", "");

        fullFilesList << removepath;
        fullFilesList.removeAt(i);
        }
    }

    fullFilesList.sort();
}
