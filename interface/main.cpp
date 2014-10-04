#include <QApplication>
#include "mainwindow.h"

int main ( int argc, char * argv[]) {

    QApplication app(argc,argv);
    MainWindow fenetre ;
    fenetre.showMaximized();

    return app.exec();
}
