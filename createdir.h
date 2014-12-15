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

private:
    Ui::CreateDir *ui;
};

#endif // CREATEDIR_H
