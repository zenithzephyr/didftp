#ifndef MONITOR_H
#define MONITOR_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class Monitor;
}

class Monitor : public QDialog
{
    Q_OBJECT

public:
    explicit Monitor(QWidget *parent = 0);
    ~Monitor();

    void initData(QStringList monitorList);

private:
    Ui::Monitor *ui;

    void createTable();

};

#endif // MONITOR_H
