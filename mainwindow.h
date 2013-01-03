#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <dslrlabview.h>
#include "ffsequence.h"

#include <QtGui>

#define DSLRLAB_DEFAULT_WIDTH               1280
#define DSLRLAB_DEFAULT_HEIGHT              720
#define DSLRLAB_SIDEBAR_RATIO               0
#define DSLRLAB_PRIMARYVIEW_RATIO           5
#define DSLRLAB_NOMARGIN                    0

#define SIDEBAR_MINIMUM_WIDTH               DSLRLAB_DEFAULT_WIDTH/5

#define LOGGING_ON                          true

#ifdef LOGGING_ON
#define LOG_MSG(...) m_pPlainTextEdit->appendPlainText(__VA_ARGS__)
#else
#define LOG_MSG(...)
#endif

class MainWindow;

namespace Ui {
class MainWindow;
}


class CustomApplication : public QApplication
{
public:
    CustomApplication(int&, char**);
    ~CustomApplication();

    // reimplemented from QApplication so we can throw exceptions in slots
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
    void actionMenuFileOpen();
    void actionMenuFileSave();
    void actionMenuFileQuit();
    void actionMenuViewFitToView();
    void actionMenuViewZoom1x();
    void actionMenuViewZoom2x();
    void actionMenuViewZoom4x();

    void actionFrameChange(int);
    void actionFrameChange(long);

    void onSequenceNew(void);
    void onSequenceClose(void);
    void onSequenceStartOpen(void);

    void onDisplayPlaneChange(int);

    void actionOpenFile(QString);
    void actionExport(QString, long, long);

    void onError(QString);
private:
    // Layouts
    QVBoxLayout                            *m_pVBoxLayout;
    QFormLayout                            *m_pOutputOptionsLayout;
    QBoxLayout                            *m_pDisplayOptionsLayout;

    // Widgets
    QWidget                                *m_pMainAnchorWidget;
    QWidget                                *m_pOutputOptionsAnchor;
    QWidget                                *m_pDisplayOptionsAnchor;

    QLabel                                 *m_pDisplayPlaneLabel;
    QComboBox                              *m_pDisplayPlaneCombo;
    DSLRLabView                            *m_pDSLRLabView;
    /////////////////////////////////////QSlider                                *m_pSlider;
    QSplitter                              *m_pSplitter;
    QToolBox                               *m_pSidebarToolBox;

    // Actions
    QActionGroup                           *m_pViewActionGroup;
    QActionGroup                           *m_pFileActionGroup;

    QAction                                *m_pActionFileOpen;
    QAction                                *m_pActionFileSave;
    QAction                                *m_pActionFileQuit;
    QAction                                *m_pActionViewFitToView;
    QAction                                *m_pActionViewZoom1x;
    QAction                                *m_pActionViewZoom2x;
    QAction                                *m_pActionViewZoom4x;
    QAction                                *m_pActionFrameChange;

    //QAction            *m_pSliderValueChanged;
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
    void updateUI(ffSequence::ffSequenceState);
};

#endif // MAINWINDOW_H
