#include "monitor.h"
#include "ui_monitor.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QFileInfo>
#include <QTextCodec>
#include <QTextDecoder>
#include <QDateTime>

Monitor::Monitor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Monitor)
{
    ui->setupUi(this);

    createTable();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    connect(this, SIGNAL(refreshMonitor()), parentWidget(), SLOT(refreshMonitor()));

    timer->start(60000);
}

Monitor::~Monitor()
{
    delete ui;
}

void Monitor::createTable()
{
    ui->monitorList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->monitorList->setColumnCount(3);
    //ui->monitorList->setRowCount(10);
    ui->monitorList->setHorizontalHeaderLabels(QStringList() << tr("Status") << tr("Time Passed") << tr("Monitor Name,Location"));// << tr("Version"));
    ui->monitorList->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->monitorList->setColumnWidth(0, 120);
    ui->monitorList->setColumnWidth(1, 160);
    ui->monitorList->horizontalHeader()->setStretchLastSection(true);
}

void Monitor::initData(QStringList monitorList)
{
    int row = 0;
    ui->monitorList->setRowCount(0);

    while(monitorList.size())
    {
    QFile *pFile = new QFile(monitorList.at(0));
    QStringList token = monitorList.at(0).split(".");
    QString mac = token.at(0);
    mac.replace(QString("_"), QString(":"));
    monitorList.removeFirst();

    if (!pFile->open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("FTP"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(mac).arg(pFile->errorString()));
        delete pFile;
    }

    QTextStream in(pFile);
    QString line;

    in.setCodec(QTextCodec::codecForName("eucKR"));

    do {
        line = in.readLine();

        if(line.startsWith("["))
        {
            QString timeStamp;
            QString monitorLoc;
            QStringList token2 = line.split("]");
            timeStamp = token2.at(0);
            monitorLoc = token2.at(1);
            timeStamp.remove(0,1);
            monitorLoc.remove(0,1);
            QDateTime time;
            QDateTime now = QDateTime::currentDateTime();
            time.setTime_t(timeStamp.toLongLong());
            unsigned int passed = now.toTime_t() -time.toTime_t();
            QString timePassed;

            timePassed.sprintf("%02d:%02d:%02d",
                    passed/3600,
                    (passed%3600)/60,
                    passed%60);

            //qDebug() << mac;
            //qDebug("%d:%d:%d",passed/3600, (passed%3600)/60, passed%60);
            //qDebug() << monitorLoc;
            ui->monitorList->insertRow(ui->monitorList->rowCount());
            QTableWidgetItem *stateItem = new QTableWidgetItem();
                    ui->monitorList->setItem(row,0, stateItem);
            QTableWidgetItem *timeItem = new QTableWidgetItem();
                    ui->monitorList->setItem(row,1, timeItem);
            QTableWidgetItem *locItem = new QTableWidgetItem();
                    ui->monitorList->setItem(row,2, locItem);
            if(passed < 10*60)
                stateItem->setText("O");
            else
                stateItem->setText("X");
            timeItem->setText(timePassed);
            locItem->setText(monitorLoc);

            int i;
            if(passed >= 24*60)
            {
                for(i=0;i<3;i++)
                ui->monitorList->item(row,i)->setBackground(Qt::red);
            }
            else if(passed >= 12*60)
            {
                for(i=0;i<3;i++)
                ui->monitorList->item(row,i)->setBackground(Qt::yellow);
            }
            row++;
        }
    } while(!line.isNull());


    pFile->close();
    delete pFile;
    }

    show();
}

void Monitor::onTimer()
{
    if(this->isVisible())
    {
        qDebug() <<"TIMER";
        emit refreshMonitor();
    }
}
