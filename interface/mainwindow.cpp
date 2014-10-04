#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(qApp, SIGNAL(lastWindowClosed()), ui->quitAction, SLOT(trigger()));
    connect(ui->quitAction, SIGNAL(), qApp, SLOT(quit()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_quitAction_triggered()
{
    qApp->quit();
}
