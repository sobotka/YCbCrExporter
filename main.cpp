/*******************************************************************************
YCbCr Lab
A tool to aid filmmakers / artists to manipulate YCbCr files to
maximize quality.
Copyright (C) 2013 Troy James Sobotka, troy.sobotka@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

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
                    "QHBoxLayout {margin: 0em 0em 0em 0em}"
                    "QProgressBar {border: 1px solid grey;"
                    "border-radius: 0; background-color: #8C8C8C;}"
                    "QProgressBar::chunk {"
                    "background-color: #404040; width: 1px;}");

    MainWindow w;
    a.m_pMainWindow = &w;
    w.show();
    
    return a.exec();
}
