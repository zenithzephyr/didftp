#include "createdir.h"
#include "ui_createdir.h"

#include <QPushButton>

CreateDir::CreateDir(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateDir)
{
    ui->setupUi(this);

    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setText(tr("Yes"));
    QPushButton *cancelButton = ui->buttonBox->button(QDialogButtonBox::Cancel);
    cancelButton->setText(tr("Cancel"));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(createClicked()));
    connect(this, SIGNAL(createNewDir(QString)), parentWidget(), SLOT(doCreateDir(QString)));
}

CreateDir::~CreateDir()
{
    delete ui;
}

void CreateDir::initData(QString &currentDir)
{
    ui->currentDirName->setText(currentDir);
    ui->newDirEdit->setText("");
}


void CreateDir::createClicked()
{
    if(!ui->newDirEdit->text().isEmpty())
    {
        emit createNewDir(ui->currentDirName->text() + "/" + ui->newDirEdit->text());
    }
}
