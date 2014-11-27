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

    void initData(QString directoryName, QStringList fileList);
    void setProgress(qint64 readBytes, qint64 totalBytes);

private:
    Ui::Upload *ui;

    QStringList removeList;
    QStringList uploadList;

    void makePlaylist();

private slots:
    void addClicked();
    void deleteClicked();
    void transmitClicked();

signals:
    void removeFiles(QString, QStringList);
    void uploadFiles(QString, QStringList);
};

#endif // UPLOAD_H
