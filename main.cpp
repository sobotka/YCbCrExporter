#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    CustomApplication a(argc, argv);
    a.setStyleSheet("QGroupBox::title {subcontrol-position:top left; "
                    "background:transparent;padding: 5 5px;} "
                    "QGroupBox {border:1px solid grey; "
                    "border-radius:6px; padding: 1.4em 0.6em 0.6em "
                    "0.6em} QVBoxLayout {margin:0em 0em 0em 0em} "
                    "QHBoxLayout {margin: 0em 0em 0em 0em}");
                    //"QLayout (margin: 0em 0em 0em 0em}");
    MainWindow w;
    a.m_pMainWindow = &w;
    w.show();
    
    return a.exec();
}
