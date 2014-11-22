#include "mainwindow.h"
#include "monitor.h"
#include "ui_mainwindow.h"
#include "upload.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createMenus();
    createTable();
    updateStatusBar();

    connect(ui->ftpList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(showUpload(QModelIndex)));
}

MainWindow::~MainWindow()
{
    delete ui;
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
    Monitor *monitor = new Monitor();

    monitor->show();
}

void MainWindow::showUpload(QModelIndex index)
{
    qDebug()<<index;

    //check ftp directory
    Upload *upload = new Upload();

    upload->show();
}

void MainWindow::createTable()
{
    ui->ftpList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ftpList->setColumnCount(4);
    ui->ftpList->setRowCount(4);
    ui->ftpList->setHorizontalHeaderLabels(QStringList() << tr("Directory") << tr("Download") << tr("File") << tr("Subtitle"));
    ui->ftpList->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->ftpList->horizontalHeader()->setStretchLastSection(true);
   // ui->ftpList->verticalHeader()->setStretchLastSection(true);

}
