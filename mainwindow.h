#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

#include "dslrlabview.h"
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

#define DSLRLAB_DEFAULT_WIDTH               1280
#define DSLRLAB_DEFAULT_HEIGHT              720
#define DSLRLAB_SIDEBAR_RATIO               0
#define DSLRLAB_PRIMARYVIEW_RATIO           5

#define SIDEBAR_MINIMUM_WIDTH               DSLRLAB_DEFAULT_WIDTH/5

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
    void onExport(QString, long, long);
    void onError(QString);
    void onStateChanged(ffSequence::ffSequenceState);
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

    QComboBox                              *m_pSBEPlaneCombo;
    QSpinBox                               *m_pSBEInSpin;
    QSpinBox                               *m_pSBEOutSpin;
    QPushButton                            *m_pSBEInReset;
    QPushButton                            *m_pSBEOutReset;

    QComboBox                              *m_pSBVPlaneCombo;

    DSLRLabView                            *m_pDSLRLabView;
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
