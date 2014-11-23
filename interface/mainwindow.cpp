#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newknitdialog.h"
#include <QMessageBox>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QFileDialog>
#include <QDebug>

#include <iostream>
#include <fstream>
#include "representation.h"

extern FILE* yyin; // from Flex
extern int yyparse(void); // from Bison
extern Knit knit_parsed;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , patternMapper(this)
{
    ui->setupUi(this);
    newDlg = new newKnitDialog(this);

    // Setup the save dialog window
    saveDlg = new QMessageBox(this);
    saveDlg->setText("Are you sure you want to quit without saving your knit ?");
    saveDlg->addButton(QString("Cancel"), QMessageBox::RejectRole);
    saveDlg->addButton(QString("Save"), QMessageBox::YesRole);
    saveDlg->addButton(QString("Close without saving"), QMessageBox::NoRole);
    connect(saveDlg, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(saveDlgTreatButton(QAbstractButton*)));

    // Connections
    connect(qApp, SIGNAL(lastWindowClosed()), ui->quitAction, SLOT(trigger()));
    connect(newDlg, SIGNAL(newKnit()), this, SLOT(newKnit()));

    // Variables
    isSaved = true;
    act = NOTHING;
    path = QDir::homePath(); // set the path to home directory

    // pattern buttons
    patternMapper.setMapping(ui->pushButton, &garter_stitch);
    connect(ui->pushButton, SIGNAL(clicked()), &patternMapper, SLOT(map()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// QUIT

void MainWindow::on_quitAction_triggered()
{
    act = QUIT;
    if (!isSaved) {
        saveDlg->show();
    } else {
        qApp->quit();
    }
}

// NEW

void MainWindow::on_newAction_triggered()
{
    act = NEW;
    if (!isSaved) {
        saveDlg->show();
    } else {
        newDlg->show();
    }
}

void MainWindow::newKnit()
{
    isSaved = false;
    QString choice = newDlg->getChoice();
    // Now just put the choice in the instructions window
    ui->instrLabel->setHtml(newDlg->getChoice());
    act = NOTHING;
}

// OPEN

void MainWindow::on_openAction_triggered()
{
    open();
}

void MainWindow::open()
{
    act = OPEN;
    if (!isSaved) {
        saveDlg->show();
    } else {
        QString file = QFileDialog::getOpenFileName(this, "Load a knit", path, "knit (*.tricot)");
        if (file.endsWith(".tricot")) {
            fileName = file;
            yyin = fopen((fileName.toStdString()).c_str(), "r");
            qDebug() << "Ok";
            int bison_return_code = yyparse();
            switch (bison_return_code) {
            case 0: {
                setInterface();
                break;
            }
            case 1: {
                QMessageBox::warning(this,"Unable to open","Unable to open the file. Parsing error.");
                break;
            }
            case 2: {
                QMessageBox::warning(this,"Unable to open","Unable to open the file. Memory exhaustion.");
                break;
            }
            }
        } else if (file != "") {
            QMessageBox::warning(this, "Wrong file format", "The selected file is not of format .tricot");
        }
        act = NOTHING;
    }
}

void MainWindow::setInterface()
{
    ui->instrLabel->setHtml(QString::fromStdString(knit_parsed.description));
    auto* v = ui->patternView;
    QGraphicsScene* scene = new QGraphicsScene();
    v->setScene(scene);
    Element* e1 = knit_parsed.elements["my_piece"];
    attachItems(e1, scene, this);

    v->setRenderHint(QPainter::HighQualityAntialiasing);
    v->scale(2, 2);
    v->update();
    v->setUpdatesEnabled(true);
//  v->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    isSaved = true;
}

// SAVE

void MainWindow::saveDlgTreatButton(QAbstractButton* b)
{
    QMessageBox::ButtonRole role = saveDlg->buttonRole(b);
    if (role == QMessageBox::RejectRole) { // do nothing, whatever the action asked
        act = NOTHING;
    } else if (role == QMessageBox::YesRole) { // save and continue
        save();
        isSaved = true;
        doSaveDlgAction();
    } else if (role == QMessageBox::NoRole) { // continue
        doSaveDlgAction();
    }
}

void MainWindow::doSaveDlgAction()
{
    switch (act) {
    case NEW: {
        newDlg->show();
        break;
    }
    case OPEN: {
        open();
        act = NOTHING;
        break;
    }
    case QUIT: {
        close();
        break;
    }
    case NOTHING: {
        // do nothing
        break;
    }
    }
}

void MainWindow::save()
{
    if (fileName != "") {
        std::ofstream save_file((fileName.toStdString()).c_str());
        if (save_file.is_open()) {
            save_file << knit_parsed;
            save_file.close();
        } else {
            QMessageBox::warning(this, "Unable to save", "I'm unable to save your knit...");
        }
    } else { // knit not already saved
        saveAs();
    }
}

void MainWindow::on_saveAction_triggered()
{
    if (!isSaved) {
        save();
        isSaved = true;
    }
}

void MainWindow::on_saveAsAction_triggered()
{
    saveAs();
}

void MainWindow::saveAs()
{
    if (fileName != "") { // set a smarter path
        path = fileName;
    }
    fileName = QFileDialog::getSaveFileName(this, "Save your knit", path, "knit (*.tricot)");
    if (fileName != "") {
        if (fileName.endsWith(".tricot")) {
            save();
        } else {
            QMessageBox::warning(this, "Wrong file format", "You don't save your tricot to the good format (.tricot)");
        }
    } else { // nothing to do
        act = NOTHING;
    }
}

// INSTRUCTIONS

void MainWindow::on_instructionsAction_triggered()
{
    /************************************
     * The building instructions code   *
     * Interactions with the Ocaml part *
     * **********************************/
}

// ABOUT

void MainWindow::on_aboutTricompAction_triggered()
{
    QMessageBox::information(this, "A propos de TriComp",
                             "TriComp est un assistant pour tricoteur. \n Plus d'informations sur \n https://github.com/TriComp");
}
