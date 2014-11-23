#include "monitor.h"
#include "ui_monitor.h"

#include <QDebug>

Monitor::Monitor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Monitor)
{
    ui->setupUi(this);

    createTable();
}

Monitor::~Monitor()
{
    delete ui;

    qDebug()<<"Monitor deleted.";
}

void Monitor::createTable()
{
    ui->monitorList->setColumnCount(4);
    ui->monitorList->setRowCount(10);
    ui->monitorList->setHorizontalHeaderLabels(QStringList() << tr("Status") << tr("Time Passed") << tr("Monitor Name,Location") << tr("Version"));
    ui->monitorList->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->monitorList->horizontalHeader()->setStretchLastSection(true);
}
