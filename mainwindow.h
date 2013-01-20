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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

#include "ycbcrlabview.h"
#include "ffsequence.h"

enum MarginDefault
{
    NOMARGIN = 0,
    LEFTMARGIN = 20
};

enum SpacingDefault
{
    BASESPACING = 14
};

#define YCBCRLAB_DEFAULT_WIDTH               1280
#define YCBCRLAB_DEFAULT_HEIGHT              720
#define YCBCRLAB_SIDEBAR_RATIO               0
#define YCBCRLAB_PRIMARYVIEW_RATIO           5

#define SIDEBAR_MINIMUM_WIDTH               YCBCRLAB_DEFAULT_WIDTH/5

class MainWindow;

namespace Ui {
class MainWindow;
}


class CustomApplication : public QApplication
{
public:
    CustomApplication(int&, char**);
    ~CustomApplication();

    // Reimplemented from QApplication so we can throw exceptions in slots.
    virtual bool notify(QObject*, QEvent*);

    MainWindow              *m_pMainWindow;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onMenuFileOpen();
    void onMenuFileExport();
    void onMenuFileQuit();
    void onMenuViewFitToView();
    void onMenuViewZoom1x();
    void onOpenFile(QString);
    void onExport(QString);
    void onError(QString);
    void onStateChanged(ffSequenceState);
    void onTrimChanged(long, long, void *);
    void onFrameChanged(long, void *);

    void onSidebarViewerPlaneChanged(int);
    void onSidebarExportPlaneChanged(int);
    void onSidebarSetIn(int);
    void onSidebarSetOut(int);
    void onSidebarResetIn(void);
    void onSidebarResetOut(void);

private:
    QToolBox                               *m_pSBToolBox;

    QPushButton                            *m_pSBEPathButton;
    QComboBox                              *m_pSBEFileTypeCombo;
    QComboBox                              *m_pSBEPlaneCombo;
    QSpinBox                               *m_pSBEInSpin;
    QSpinBox                               *m_pSBEOutSpin;
    QPushButton                            *m_pSBEInReset;
    QPushButton                            *m_pSBEOutReset;

    QComboBox                              *m_pSBVPlaneCombo;

    YCbCrLabView                           *m_pYCbCrLabView;
    QSplitter                              *m_pSplitter;

    // Actions
    QActionGroup                           *m_pViewActionGroup;
    QActionGroup                           *m_pFileActionGroup;

    QAction                                *m_pActionFileOpen;
    QAction                                *m_pActionFileExport;
    QAction                                *m_pActionFileQuit;
    QAction                                *m_pActionViewFitToView;
    QAction                                *m_pActionViewZoom1x;
    QAction                                *m_pActionSetIn;
    QAction                                *m_pActionSetOut;
    QAction                                *m_pActionResetIn;
    QAction                                *m_pActionResetOut;

    // Menus
    QMenu                                  *m_pMenuFile;
    QMenu                                  *m_pMenuView;

    // Other
    QFuture<void>                           m_workerThread;

    void openFile(void);
    void exportFile(void);
    void createObjects(void);
    void initObjects(void);
    void initSidebar(void);
    void createActions(void);
    void createMenus(void);
};

#endif // MAINWINDOW_H
