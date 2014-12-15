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

private:
    Ui::Subtitle *ui;
};

#endif // SUBTITLE_H
