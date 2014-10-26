#include "newknitdialog.h"
#include "ui_newknitdialog.h"
#include <QMessageBox>

newKnitDialog::newKnitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newKnitDialog)
{
    ui->setupUi(this);
    knit = QString::fromStdString("Coucou");
}

newKnitDialog::~newKnitDialog()
{
    delete ui;
}

void newKnitDialog::createNewKnit(QAbstractButton* b)
{
    QDialogButtonBox::StandardButton role = ui->buttonBox->standardButton(b);
    if (role == QDialogButtonBox::Cancel) {
       // do nothing
    }
    else if (role == QDialogButtonBox::Ok) {
       QMessageBox::information(this,"Your choice",knit);
    }
}

void newKnitDialog::changeChoice()
{
    knit = ui->choiceBox->currentText();
}

QString newKnitDialog::getChoice()
{
    return knit;
}
