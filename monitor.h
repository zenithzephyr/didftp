#ifndef MONITOR_H
#define MONITOR_H

#include <QDialog>
#include <QStringList>
#include <QTimer>

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

    QTimer *timer;
private slots:
    void onTimer();

signals:
    void refreshMonitor();
};

#endif // MONITOR_H
