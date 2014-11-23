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

    void initData(QString directoryName);

private:
    Ui::Upload *ui;

    QStringList removeList;
    QStringList uploadList;

private slots:
    void addClicked();
    void deleteClicked();
    void transmitClicked();

signals:
    void removeFiles(QStringList);
    void uploadFiles(QStringList);
};

#endif // UPLOAD_H
