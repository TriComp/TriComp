#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "newknitdialog.h"

namespace Ui {
  class MainWindow /*: public Ui::MainWindow {}*/;
}

class MainWindow : public QMainWindow //, public Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_quitAction_triggered();
    void on_aboutTricompAction_triggered();
    void on_instructionsAction_triggered();
    void on_newAction_triggered();
    void on_openAction_triggered();
    void newKnit();

private:
    Ui::MainWindow *ui;
    newKnitDialog *newDlg;
    bool saveDone;

};

#endif // MAINWINDOW_H
