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
QStringList upList;
QStringList rmList;
QString currentDirectory("");
QString currentPath;
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
    connect(refreshAction, SIGNAL(triggered()), this, SLOT(refreshTable()));
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
    QMap<QString, qint64> tmpFiles;

    //check ftp directory

    QTableWidgetItem *item = ui->ftpList->item(index.row(),0);
    if(item && !item->text().isEmpty())
    {
        int i;
        QStringList directoryFiles;
        QString token = item->text();

        token.remove(0,1);
        token.append(QChar('/'));
        qDebug()<<token;
        if(index.row() == 0) //root dir
        {
            for(i=0;i<fullFilesList.size();i++)
            {
                if(!fullFilesList.at(i).contains("/"))
                {
                    directoryFiles << fullFilesMap[fullFilesList.at(i)].name();
                    tmpFiles[fullFilesMap[fullFilesList.at(i)].name()] = fullFilesMap[fullFilesList.at(i)].size();
                }
            }
        }
        else
        {
            for(i=0;i<fullFilesList.size();i++)
            {
                if(fullFilesList.at(i).contains(token) &&
                                (fullFilesList.at(i).count("/") == token.count("/")) && fullFilesList.at(i) != token)
                {
                    directoryFiles << fullFilesMap[fullFilesList.at(i)].name();
                    tmpFiles[fullFilesMap[fullFilesList.at(i)].name()] = fullFilesMap[fullFilesList.at(i)].size();
                }
            }
        }
        upload->initData(item->text(), directoryFiles, tmpFiles);
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
    ui->ftpList->setColumnWidth(0, 120);
    ui->ftpList->setColumnWidth(1, 160);
    ui->ftpList->setColumnWidth(2, 260);
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

    if(ftp->currentCommand() == QFtp::Put)
    {
        qDebug()<<"PUT END";

        file->close();
        delete file;

        if(upList.size())
        {
            doUpload();
        }
        else
        {
            refreshTable(); //FIXME
            upload->close();
        }
    }

    if(ftp->currentCommand() == QFtp::Remove)
    {
        qDebug()<<"DELETE END";

        if(rmList.size())
            doRemove();
    }
}

void MainWindow::refreshTable()
{
    unparsedDirectory.clear();
    fullFilesList.clear();
    playlistPath.clear();
    upList.clear();
    rmList.clear();
    currentDirectory="";
    currentPath.clear();
    fullFilesMap.clear();
    TimeMap.clear();
    playlistMap.clear();
    num = 0;
    row = 0;

    ui->ftpList->clear();
    ui->ftpList->setHorizontalHeaderLabels(QStringList() << tr("Directory") << tr("Download") << tr("File") << tr("Subtitle"));

    ftp->list();
}

void MainWindow::updateDataTransferProgress(qint64 readBytes,
                                           qint64 totalBytes)
{
  //  qDebug()<<"updateDataTransferProgress";
  //  qDebug("Total : %ld, read : %ld",totalBytes, readBytes);
    upload->setProgress(readBytes, totalBytes);
}

void MainWindow::removeFiles(QString path, QStringList removeList)
{
    int i;
    qDebug()<<"removeFiles";

    for(i=0;i<removeList.size();i++)
        rmList << removeList.at(i);

    currentPath = path;

    if(rmList.size())
        doRemove();
}

void MainWindow::doRemove()
{
    ftp->remove(currentPath+"/"+rmList.at(0));

    rmList.removeFirst();
}

void MainWindow::uploadFiles(QString path, QStringList uploadList)
{
    qDebug()<<"uploadFiles";

    int i;
    for(i=0;i<uploadList.size();i++)
        upList << uploadList.at(i);

    currentPath = path;

    if(upList.size())
        doUpload();
}

void MainWindow::doUpload()
{
    file = new QFile(upList.at(0));

    if (!file->open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("FTP"),
                                 tr("Unable to read the file %1: %2.")
                                 .arg(file->fileName()).arg(file->errorString()));
        delete file;
    }

    ftp->put(file, currentPath+"/"+QFileInfo(file->fileName()).fileName());

    upList.removeFirst();
}

void MainWindow::makeTableData()
{
    int i,j;
    QTableWidgetItem *item = new QTableWidgetItem("/"); //root
    ui->ftpList->setItem(0,0,item);

    if(!TimeMap["/"].isEmpty())
    {
        QTableWidgetItem *item2 = new QTableWidgetItem(TimeMap["/"]);
        ui->ftpList->setItem(0,1,item2);
    }

    for(i=0;i<4;i++)
    {
        if(ui->ftpList->item(0,i) == NULL)
            ui->ftpList->setItem(0,i, new QTableWidgetItem);
        ui->ftpList->item(0,i)->setBackground(Qt::gray);
    }

    for(i=0;i<fullFilesList.size();i++)
    {
        if(!fullFilesList.at(i).contains("/"))
        {
            row++;
            QTableWidgetItem *item = new QTableWidgetItem(fullFilesMap[fullFilesList.at(i)].name());
            ui->ftpList->setItem(row,2,item);
        }
    }

    for(i=0;i<fullFilesList.size();i++)
    {
        if(!fullFilesList.at(i).contains("/"))
        {
            row--;
            continue;
        }

        if(fullFilesList.at(i).endsWith("/")) //directory
        {
            QTableWidgetItem *item = new QTableWidgetItem("/"+fullFilesMap[fullFilesList.at(i)].name());

            ui->ftpList->setItem(i+row+1,0,item);
            if(!TimeMap[fullFilesList.at(i)].isEmpty())
            {
            QTableWidgetItem *item2 = new QTableWidgetItem(TimeMap[fullFilesList.at(i)]);
            ui->ftpList->setItem(i+row+1,1,item2);
            }

            for(j=0;j<4;j++)
            {
                if(ui->ftpList->item(i+row+1,j) == NULL)
                    ui->ftpList->setItem(i+row+1,j, new QTableWidgetItem);
                ui->ftpList->item(i+row+1,j)->setBackground(Qt::gray);
            }
        }
        else //files
        {
            QTableWidgetItem *item = new QTableWidgetItem(fullFilesMap[fullFilesList.at(i)].name());
            ui->ftpList->setItem(i+row+1,2,item);
        }
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
                                     tr("Unable to write the file %1: %2.")
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

            clist.remove(clist.size()-1,1);
            dtime.setTime_t(clist.toInt());
            TimeMap[path] =dtime.toString("yyyy/MM/dd hh:mm:ss");
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
            line.remove(0,1);
            //cut string
            QStringList clist = line.split(",");
            playlistFiles << clist.at(0);
            qDebug()<<"File : " << clist.at(0);
        }
    } while(!line.isNull());


    pFile->close();
    delete pFile;

    int i, j;
    //FIXME
    QMutableStringListIterator iter(fullFilesList);

    while(iter.hasNext())
    {
        bool removeFlag = false;
        QString next = iter.next();
        for(j=0;j<playlistFiles.size();j++)
        {
            if(next.contains(playlistFiles.at(j)) && !next.contains("///"))
            {
                QString addpath = next + "///";

                iter.insert(addpath);
                removeFlag = true;
            }
        }

        qDebug() << "PATH : " << path;
        if(path == "/" && next.count("/") == 0)
            removeFlag = true;

        if(next.startsWith(path) && (next.count("/") == path.count("/")) && next != path)
        {
            qDebug() << "DELETED : " << next;
            removeFlag = true;
        }

        if(removeFlag)
            iter.remove();
    }

    iter.toFront();
    while(iter.hasNext())
    {
        QString next = iter.next();
        if(next.contains("///"))
        {
            QString removepath = next;
            removepath.replace("///", "");

            iter.insert(removepath);
            iter.remove();
        }
    }

    fullFilesList.sort();
}
