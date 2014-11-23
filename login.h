#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "qurlinfo.h"

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

private:
    Ui::Login *ui;

public:
    void writeInfo();
    void readInfo();

private slots:
    void connectFtp();

signals:
    void sendConnectInfo(QString,QString,QString);
};

#endif // LOGIN_H
