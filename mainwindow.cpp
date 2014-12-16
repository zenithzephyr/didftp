#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileInfo>
#include <QTextCodec>
#include <QTextDecoder>

QStringList unparsedDirectory;
QStringList fullFilesList;
QStringList fullDirList;
QStringList playlistPath;
QStringList subtitlePath;
QStringList upList;
QStringList rmList;
QString currentDirectory("");
QString currentPath;
QMap<QString, QUrlInfo> fullFilesMap;
QMap<QString, QString> TimeMap;
QMap<QString, QString> SubTimeMap;
QMap<QString, QString> playlistMap;
QMap<QString, QString> subtitleMap;
QStringList delDirList;
QStringList delFileList;
QStringList unparsedDelDir;
QStringList undeletedList;
QString deleteRoot;

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
    createdir = new CreateDir(this);
    subtitle = new Subtitle(this);
    ftp = new QFtp();

    login->show();

    ui->ftpList->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->ftpList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
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
    controlMenu->addSeparator();
    controlMenu->addAction(controlAction);
    controlMenu->addAction(upgradeAction);
    controlMenu->addSeparator();
    controlMenu->addAction(exitAction);
}

void MainWindow::createActions()
{
    refreshAction = new QAction(tr("&Refresh"), this);
    controlAction = new QAction(tr("&Control Monitor"), this);
    upgradeAction = new QAction(tr("&Upgrade Monitor"), this);
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

void MainWindow::showUpload(QModelIndex index) //FIXME : my name
{
    //qDebug()<<index;
    QMap<QString, qint64> tmpFiles;

    //check ftp directory

    QTableWidgetItem *item = ui->ftpList->item(index.row(),0);
    if(item && !item->text().isEmpty())
    {
        if(index.column() == 3) //subtitle
        {
            QTableWidgetItem *subitem = ui->ftpList->item(index.row(),3);
            subtitle->initData(subitem->text(),item->text());
            subtitle->show();
        }
        else
        {
        int i;
        QStringList directoryFiles;
        QString token = item->text();

        token.remove(0,1);
        token.append(QChar('/'));
        //qDebug()<<token;
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
    if(ftpmode == DELETEDIR)
    {
        if(url.isDir() && !url.isSymLink()) //directory
        {
            unparsedDelDir << currentDirectory + "/" + url.name();
            delDirList << currentDirectory + "/" + url.name();
        }
        else //file
        {
            delFileList << currentDirectory + "/" + url.name();
        }
    }
    else //NORMAL
    {
        //qDebug()<<url.name();

        if(url.isDir() && !url.isSymLink()) //directory
        {
            if(currentDirectory.isEmpty()) //root directory
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
                else if(url.name() == "subtitle.txt")
                    subtitlePath << "/";

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
                else if(url.name() == "subtitle.txt")
                    subtitlePath << currentDirectory + "/";

                fullFilesList << currentDirectory + "/" + url.name();
                fullFilesMap[currentDirectory + "/" + url.name()] = url;
            }
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

        ftpmode = NORMAL;
        ftp->list(); //get all list
        currentDirectory = "";

        login->hide();
        show();

        return;
    }

    if (ftp->currentCommand() == QFtp::List) //Recrusive
    {
        if(ftpmode == DELETEDIR)
        {
            if(unparsedDelDir.size())
            {
                ftpmode = DELETEDIR;
                ftp->list(unparsedDelDir.at(0));
                currentDirectory = unparsedDelDir.at(0);
                unparsedDelDir.removeFirst();
            }
            else
            {
                int i,j;
                delDirList.sort();
                delFileList.sort();

                if(delDirList.size())
                {
                    for(i=delDirList.size()-1;i>=0;i--)
                    {
                        for(j=delFileList.size()-1;j>=0;j--)
                        {
                            if(delFileList.at(j).contains(delDirList.at(i)))
                                undeletedList << delFileList.at(j);
                        }
                        undeletedList << delDirList.at(i) + "/";
                    }

                }

                for(i=delFileList.size()-1;i>=0;i--)
                {
                    undeletedList << delFileList.at(i);
                }

                undeletedList << deleteRoot + "/";

#if 0
                for(i=0;i<delDirList.size();i++)
                    qDebug() << delDirList.at(i);
                for(i=0;i<delFileList.size();i++)
                    qDebug() << delFileList.at(i);
#endif
                for(i=0;i<undeletedList.size();i++)
                    qDebug() << undeletedList.at(i);


                QString dir = undeletedList.at(0);
                dir.remove(undeletedList.at(0).size()-1,1);
                if(undeletedList.at(0).endsWith("/")) //directory
                {
                    qDebug()<<"DEL DIR "<< dir;
                    ftp->rmdir(dir);
                }
                else
                {
                    qDebug()<<"DEL FILE "<<undeletedList.at(0);
                    ftp->remove(undeletedList.at(0));
                }

                undeletedList.removeFirst();
            }
        }
        else
        {
            if(unparsedDirectory.size())
            {
                ftpmode = NORMAL;
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
        if(ftpmode == GETPLAY)
            getPlaylist();
        else if(ftpmode == GETSUB)
            getSubtitle();
    }

    if(ftp->currentCommand() == QFtp::Put)
    {
        //qDebug()<<"PUT END";

        file->close();
        delete file;

        if(ftpmode == SUBTITLE)
        {
            refreshTable(); //FIXME
            subtitle->close();
        }
        else
        {
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
    }

    if(ftp->currentCommand() == QFtp::Remove)
    {
        //qDebug()<<"DELETE END";
        if(ftpmode == DELETEDIR)
        {
            if(undeletedList.size())
            {
                QString dir = undeletedList.at(0);
                dir.remove(undeletedList.at(0).size()-1,1);
                if(undeletedList.at(0).endsWith("/")) //directory
                {
                    ftp->rmdir(dir);
                }
                else
                {
                    ftp->remove(undeletedList.at(0));
                }

                undeletedList.removeFirst();
            }
            else
            {
                refreshTable();
            }
        }
        else
        {
            if(rmList.size())
                doRemove();
        }
    }

    if(ftp->currentCommand() == QFtp::Mkdir)
            refreshTable();

    if(ftp->currentCommand() == QFtp::Rmdir)
    {
        if(ftpmode == DELETEDIR)
        {
            if(undeletedList.size())
            {
                QString dir = undeletedList.at(0);
                dir.remove(undeletedList.at(0).size()-1,1);
                if(undeletedList.at(0).endsWith("/")) //directory
                {
                    ftp->rmdir(dir);
                }
                else
                {
                    ftp->remove(undeletedList.at(0));
                }

                undeletedList.removeFirst();
            }
            else
            {
                refreshTable();
            }

        }
    }
}

void MainWindow::refreshTable()
{
    unparsedDirectory.clear();
    fullFilesList.clear();
    fullDirList.clear();
    playlistPath.clear();
    subtitlePath.clear();
    upList.clear();
    rmList.clear();
    currentDirectory="";
    currentPath.clear();
    fullFilesMap.clear();
    TimeMap.clear();
    SubTimeMap.clear();
    playlistMap.clear();
    subtitleMap.clear();
    num = 0;
    row = 0;

    delDirList.clear();
    delFileList.clear();
    unparsedDelDir.clear();
    undeletedList.clear();
    deleteRoot="";

    ui->ftpList->clear();
    ui->ftpList->setHorizontalHeaderLabels(QStringList() << tr("Directory") << tr("Download") << tr("File") << tr("Subtitle"));

    ftpmode = NORMAL;
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

    for(i=0;i<removeList.size();i++)
        rmList << removeList.at(i);

    currentPath = path;

    if(rmList.size())
        doRemove();
}

void MainWindow::doRemove()
{
    ftpmode = DELETEFILE;
    ftp->remove(currentPath+"/"+rmList.at(0));

    rmList.removeFirst();
}

void MainWindow::uploadFiles(QString path, QStringList uploadList)
{
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

    ftpmode = UPLOAD;
    ftp->put(file, currentPath+"/"+QFileInfo(file->fileName()).fileName());

    upList.removeFirst();
}

void MainWindow::uploadSubtitle(QString path)
{
    file = new QFile("subtitle.txt");

    if (!file->open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("FTP"),
                                 tr("Unable to read the file %1: %2.")
                                 .arg(file->fileName()).arg(file->errorString()));
        delete file;
    }

    ftpmode = SUBTITLE;
    ftp->put(file, path + "/"+QFileInfo(file->fileName()).fileName());
}

void MainWindow::makeTableData()
{
    int i,j;
    QTableWidgetItem *item = new QTableWidgetItem("/"); //root
    ui->ftpList->setItem(0,0,item);
    row++;

    //debug
#if 1
    for(i=0;i<fullFilesList.size();i++)
        qDebug() << fullFilesList.at(i);
#endif
    //make fullDirList
    for(i=0;i<fullFilesList.size();i++)
    {
        if(fullFilesList.at(i).endsWith("/")) //directory
            fullDirList << fullFilesList.at(i);
    }

    int hasSubTime  = 0;

    if(!SubTimeMap["/"].isEmpty())
    {
        QTableWidgetItem *item2 = new QTableWidgetItem(SubTimeMap["/"]);
        ui->ftpList->setItem(0,1,item2);

        hasSubTime = 1;
    }

    if(!TimeMap["/"].isEmpty())
    {
        QTableWidgetItem *item2 = new QTableWidgetItem(TimeMap["/"]);
        ui->ftpList->setItem(hasSubTime,1,item2);
    }

    if(!subtitleMap["/"].isEmpty())
    {
        QString sub = getSubText("/",subtitleMap["/"]);
        QTableWidgetItem *item3 = new QTableWidgetItem(sub);
        ui->ftpList->setItem(0,3,item3);
    }

    for(i=0;i<4;i++)
    {
        if(ui->ftpList->item(0,i) == NULL)
            ui->ftpList->setItem(0,i, new QTableWidgetItem);
        ui->ftpList->item(0,i)->setBackground(Qt::gray);
    }

    for(i=0;i<fullFilesList.size();i++)
    {
        if(!fullFilesList.at(i).contains("/")) //root dir files
        {
            QTableWidgetItem *item = new QTableWidgetItem(fullFilesMap[fullFilesList.at(i)].name());
            ui->ftpList->setItem(row,2,item);
            row++;
        }
    }

    for(i=0;i<fullDirList.size();i++)
    {
        QString dir = fullDirList.at(i);
        dir.remove(fullDirList.at(i).size()-1,1);
        QTableWidgetItem *item = new QTableWidgetItem("/"+dir);
        ui->ftpList->setItem(row,0,item);

    if(!subtitleMap[fullDirList.at(i)].isEmpty())
    {
        QString sub = getSubText(fullDirList.at(i), subtitleMap[fullDirList.at(i)]);
        QTableWidgetItem *item3 = new QTableWidgetItem(sub);
        ui->ftpList->setItem(row,3,item3);
    }

        hasSubTime = 0;
        if(!SubTimeMap[fullDirList.at(i)].isEmpty())
        {
            QTableWidgetItem *item2 = new QTableWidgetItem(SubTimeMap[fullDirList.at(i)]);
            ui->ftpList->setItem(row,1,item2);
            hasSubTime = 1;
        }

        if(!TimeMap[fullDirList.at(i)].isEmpty())
        {
            QTableWidgetItem *item2 = new QTableWidgetItem(TimeMap[fullDirList.at(i)]);
            ui->ftpList->setItem(row+hasSubTime,1,item2);
        }


        for(j=0;j<4;j++)
        {
            if(ui->ftpList->item(row,j) == NULL)
                ui->ftpList->setItem(row,j, new QTableWidgetItem);
            ui->ftpList->item(row,j)->setBackground(Qt::gray);
        }
        row++;

        for(j=0;j<fullFilesList.size();j++)
        {
            if(fullFilesList.at(j).contains(fullDirList.at(i))
                            && !(fullFilesList.at(j) == fullDirList.at(i))
                            && (fullFilesList.at(j).count("/") == fullDirList.at(i).count("/")))
            {
                QTableWidgetItem *item = new QTableWidgetItem(fullFilesMap[fullFilesList.at(j)].name());
                ui->ftpList->setItem(row,2,item);
                row++;
            }
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

        ftpmode = GETPLAY;
        ftp->get(playlistPath.at(0) + "/playlist.txt",file);

        playlistMap[playlistPath.at(0)] = fileName;
        playlistPath.removeFirst();
    }
    else
    {
        getSubtitle();
    }
}

QString MainWindow::getSubText(QString path, QString fileName)
{
    QFile *pFile = new QFile(fileName);

    if (!pFile->open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("FTP"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(fileName).arg(file->errorString()));
        delete file;
    }

    QTextStream in(pFile);
    QString line;
    QString plainText;

    in.setCodec(QTextCodec::codecForName("eucKR"));

    do {
        line = in.readLine();

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
            SubTimeMap[path] =dtime.toString("yyyy/MM/dd hh:mm:ss");
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
            plainText.append(line);
        }
    } while(!line.isNull());


    pFile->close();
    delete pFile;

    return plainText;
}

void MainWindow::getSubtitle() //chain reaction
{
    num++; //FIXME
    if(subtitlePath.size())
    {
        QString fileName = QString("subtitle%1.txt").arg(num); //FIXME filename : filename must can found.
        file = new QFile(fileName);

        if (!file->open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("FTP"),
                                     tr("Unable to write the file %1: %2.")
                                     .arg(fileName).arg(file->errorString()));
            delete file;
        }

        ftpmode = GETSUB;
        ftp->get(subtitlePath.at(0) + "/subtitle.txt",file);

        subtitleMap[subtitlePath.at(0)] = fileName;
        subtitlePath.removeFirst();
    }
    else
    {
        QMapIterator<QString, QString> i(playlistMap);
        while(i.hasNext()) {
            i.next();
            //qDebug()<< i.key() << ": " << i.value() << endl;
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
            //qDebug()<<"File : " << clist.at(0);
        }
    } while(!line.isNull());


    pFile->close();
    delete pFile;

    int i;
    //FIXME
    QMutableStringListIterator iter(fullFilesList);

    while(iter.hasNext())
    {
        bool removeFlag = false;
        QString next = iter.next();
        for(i=0;i<playlistFiles.size();i++)
        {
            if(next.contains(playlistFiles.at(i)) && !next.contains("///"))
            {
                QString addpath = next + "///";

                iter.insert(addpath);
                removeFlag = true;
            }
        }

        //qDebug() << "PATH : " << path;
        if(path == "/" && next.count("/") == 0)
            removeFlag = true;

        if(next.startsWith(path) && (next.count("/") == path.count("/")) && next != path)
        {
         //   qDebug() << "DELETED : " << next;
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

void MainWindow::showContextMenu(QPoint point)
{
    QPoint globalPos = this->mapToGlobal(point);

    QModelIndex idx = ui->ftpList->currentIndex();
    QTableWidgetItem *item = ui->ftpList->item(idx.row(),0);
    if(item && !item->text().isEmpty())
    {
        QMenu menu;
        QAction	videoAction(tr("Video/Picture(&V)"), this);
        QAction	subtitleAction(tr("Subtitle(&S)"), this);
        QAction	createDirAction(tr("Create Directory(&C)"), this);
        QAction deleteDirAction(tr("Remove Directory(&D)"), this);
        menu.addAction(&videoAction);
        menu.addAction(&subtitleAction);
        menu.addSeparator();
        menu.addAction(&createDirAction);
        menu.addAction(&deleteDirAction);
        if(idx.row() == 0)
            deleteDirAction.setDisabled(true);
        QAction *selectedItem = menu.exec(globalPos);

        if(selectedItem == &videoAction) {
            showUpload(idx);
        }
        else if(selectedItem == &subtitleAction) {
            QTableWidgetItem *subitem = ui->ftpList->item(idx.row(),3);
            subtitle->initData(subitem->text(),item->text());
            subtitle->show();
        }
        else if(selectedItem == &createDirAction) {
            showCreateDir(item->text());
        }
        else if(selectedItem == &deleteDirAction) {
            QMessageBox deleteBox;
            deleteBox.setWindowTitle(tr("Delete Confirm"));
            deleteBox.setText(tr("Are you sure to remove this(%1) directory?\nIf files in it. All files deleted.").arg(item->text()));
            deleteBox.setStandardButtons(QMessageBox::Yes|QMessageBox::Cancel);
            deleteBox.setButtonText(QMessageBox::Yes, tr("Yes"));
            deleteBox.setButtonText(QMessageBox::Cancel, tr("Cancel"));

            int ret = deleteBox.exec();
            if(ret == QMessageBox::Yes)
            {
                qDebug() << item->text();
                ftpmode = DELETEDIR;
                deleteRoot = currentDirectory = (item->text());
                ftp->list(item->text());
                //ftp->rmdir(item->text());
            }
        }
    }
}

void MainWindow::showCreateDir(QString currentDir)
{
    createdir->initData(currentDir);
    createdir->show();
}

void MainWindow::doCreateDir(QString dir)
{
    ftp->mkdir(dir);
}
