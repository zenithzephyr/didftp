#ifndef SUBTITLE_H
#define SUBTITLE_H

#include <QDialog>

namespace Ui {
class Subtitle;
}

class Subtitle : public QDialog
{
    Q_OBJECT

public:
    explicit Subtitle(QWidget *parent = 0);
    ~Subtitle();

    void initData(QString subtext, QString path);

private:
    Ui::Subtitle *ui;

    void makeSubtitle();

private slots:
    void transmitClicked();

signals:
    void uploadSubtitle(QString);
};

#endif // SUBTITLE_H
