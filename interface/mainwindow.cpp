#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newknitdialog.h"
#include <QMessageBox>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <representation.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    newDlg = new newKnitDialog(this);

    // Setup a simple save dialog window
    saveDlg = new QMessageBox(this);
    saveDlg->setText("Are you sure you want to quit without saving your knit ?");
    saveDlg->addButton(QString("Cancel"), QMessageBox::RejectRole);
    saveDlg->addButton(QString("Save"), QMessageBox::YesRole);
    saveDlg->addButton(QString("Do not save"),QMessageBox::NoRole);
    connect(saveDlg,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(saveDlgTreatButton(QAbstractButton*)));

    // Connections
    connect(qApp, SIGNAL(lastWindowClosed()), ui->quitAction, SLOT(trigger()));
    connect(newDlg,SIGNAL(newKnit()),this,SLOT(newKnit()));

    // Variables
    isSaved = true;
    act = NOTHING;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_quitAction_triggered()
{
    act = QUIT ;
    if (!isSaved) {
        saveDlg->show();
    }
    else {
        qApp->quit();
    }
}

// NEW

void MainWindow::on_newAction_triggered()
{
    act = NEW ;
    if (!isSaved) {
        saveDlg->show();
    }
    else {
        newDlg->show();
    }
}

void MainWindow::newKnit()
{
    isSaved = false ;
    QString choice = newDlg->getChoice();
    // Now just put the choice in the instructions window
    ui->instrLabel->setHtml(newDlg->getChoice());
    act = NOTHING ;
}

void MainWindow::on_openAction_triggered()
{

    auto *v = ui->patternView;
    QGraphicsScene *scene = new QGraphicsScene();
    v->setScene(scene);

    auto *stop1 = new Stop();
    auto *stop2 = new Stop();
    auto *l = new TrapezoidElem(Trapezoid{120, -13, 60, 22}, stop1);
    auto *r = new TrapezoidElem(Trapezoid{120, 22, 120, 100}, stop2);
    auto *split = new Split(l, r, 60);
    auto *e2 = new TrapezoidElem(Trapezoid{120, -13, 180, 240}, split);
    auto *e1 = new TrapezoidElem(Trapezoid{100, 30, 200, 180}, e2);

    attachItems(e1, scene);

    v->setRenderHint(QPainter::HighQualityAntialiasing);


    v->update();
//    v->setUpdatesEnabled(true);
//    v->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);


    if(!isSaved) {
        saveDlg->show();
    }
    else {
        open();
    }
}

void MainWindow::open()
{
    // open, just to check the program works
    //ui->instrLabel->setHtml("You want to open");

    /*************************\
     * The opening file code *
     *************************/
    act = NOTHING ;


}

void MainWindow::saveDlgTreatButton(QAbstractButton* b)
{
    QMessageBox::ButtonRole role = saveDlg->buttonRole(b);
    if (role == QMessageBox::RejectRole) { // do nothing, whatever the action asked
        act = NOTHING ;
    }
    else if (role == QMessageBox::YesRole) { // save and continue
        save();
        isSaved = true ;
        doSaveDlgAction();
    }
    else if (role == QMessageBox::NoRole) { // continue
        doSaveDlgAction();
    }
}

void MainWindow::doSaveDlgAction()
{
    switch (act) {
        case NEW : {
            newDlg->show();
            break;
        }
        case OPEN : {
            open() ;
            act = NOTHING ;
            break;
        }
        case QUIT : {
            close();
            break;
        }
        case NOTHING : {
            // do nothing
            break;
        }
    }
}

void MainWindow::save()
{
    // Just to check the program works
    ui->instrLabel->setHtml(QString::fromStdString("You want to save"));

    /************************
     * The saving file code *
     * **********************/
}

void MainWindow::on_saveAction_triggered()
{
    if (!isSaved) {
        save();
        isSaved = true ;
    }
}

void MainWindow::on_saveAsAction_triggered()
{
    // choose filename and save
}

void MainWindow::on_instructionsAction_triggered()
{
    /**********************************
     * The building instructions code *
     * ********************************/
}

void MainWindow::on_aboutTricompAction_triggered()
{
    QMessageBox::information(this,"A propos de TriComp",
    "TriComp est un assistant pour tricoteur. \n Plus d'informations sur \n https://github.com/TriComp");
}
