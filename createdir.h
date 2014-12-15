#ifndef CREATEDIR_H
#define CREATEDIR_H

#include <QDialog>

namespace Ui {
class CreateDir;
}

class CreateDir : public QDialog
{
    Q_OBJECT

public:
    explicit CreateDir(QWidget *parent = 0);
    ~CreateDir();

    void initData(QString &currentDir);
private:
    Ui::CreateDir *ui;

private slots:
    void createClicked();

signals:
    void createNewDir(QString);
};

#endif // CREATEDIR_H
