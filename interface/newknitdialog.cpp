#include "newknitdialog.h"
#include "ui_newknitdialog.h"
#include <QMessageBox>

newKnitDialog::newKnitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newKnitDialog)
{
    ui->setupUi(this);
}

newKnitDialog::~newKnitDialog()
{
    delete ui;
}

