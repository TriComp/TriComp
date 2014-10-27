#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newknitdialog.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    newDlg = new newKnitDialog(this);

    connect(qApp, SIGNAL(lastWindowClosed()), ui->quitAction, SLOT(trigger()));
    connect(ui->quitAction, SIGNAL(), qApp, SLOT(quit()));
    connect(newDlg,SIGNAL(newKnit()),this,SLOT(newKnit()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_quitAction_triggered()
{
    qApp->quit();
}

void MainWindow::on_newAction_triggered()
{
    newDlg->show();
}

void MainWindow::newKnit()
{
    QString choice = newDlg->getChoice();
    // Now just put the choice in the instructions window
    ui->instrLabel->setHtml(newDlg->getChoice());
}

void MainWindow::on_openAction_triggered()
{

}

void MainWindow::on_aboutTricompAction_triggered()
{
    QMessageBox::information(this,"A propos de TriComp",
    "TriComp est un assistant pour tricoteur. \n Plus d'informations sur \n https://github.com/TriComp");
}


void MainWindow::on_instructionsAction_triggered()
{
    // Build the instructions
}
