#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
//    w.show();

    w.setWindowFlag(Qt::FramelessWindowHint);
    w.showFullScreen(); //opens the application in fullscreen
    return a.exec();
}
