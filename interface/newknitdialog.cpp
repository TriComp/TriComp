#include "newknitdialog.h"
#include "ui_newknitdialog.h"
#include <QMessageBox>

newKnitDialog::newKnitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newKnitDialog)
{
    ui->setupUi(this);
    knit = QString();
}

newKnitDialog::~newKnitDialog()
{
    delete ui;
}

void newKnitDialog::ok()
{
    emit newKnit();
    close();
}

void newKnitDialog::changeChoice()
{
    knit = ui->choiceBox->currentText();
}

QString newKnitDialog::getChoice()
{
    return knit;
}
