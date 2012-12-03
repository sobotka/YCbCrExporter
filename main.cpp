#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    CustomApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
