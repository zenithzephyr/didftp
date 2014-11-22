#ifndef UPLOAD_H
#define UPLOAD_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class Upload;
}

class Upload : public QDialog
{
    Q_OBJECT

public:
    explicit Upload(QWidget *parent = 0);
    ~Upload();

private:
    Ui::Upload *ui;

    //QStringList ftpFiles;
    //QStringList localFiles;
    QStringList removeList;

    void updateList();

private slots:
    void addClicked();
    void deleteClicked();
    void transmitClicked();
};

#endif // UPLOAD_H
