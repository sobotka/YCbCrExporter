#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <dslrlabview.h>
#include <ffsequence.h>

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSplitter>
#include <QAction>
#include <QMenu>
#include <QPlainTextEdit>
#include <QMessageBox>

#define DSLRLAB_DEFAULT_WIDTH               1000
#define DSLRLAB_DEFAULT_HEIGHT              562
#define DSLRLAB_SIDEBAR_RATIO               0
#define DSLRLAB_PRIMARYVIEW_RATIO           5
#define DSLRLAB_NOMARGIN                    0

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
    void actionMenuFileQuit();
    void actionMenuViewFitToView();
    void actionMenuViewZoom1x();
    void actionMenuViewZoom2x();
    void actionMenuViewZoom4x();

    void actionFrameChange(int);
    void actionFrameChange(long);

    void actionSequenceNew(void);

    void actionOpenFile(QString);

    void actionError(QString);
private:
    // Layouts
    QVBoxLayout                            *m_pVBoxLayout;
    QVBoxLayout                            *m_pSidebarLayout;
    // Widgets
    QWidget                                *m_pMainAnchorWidget;
    QWidget                                *m_pUpperAnchorWidget;
    QWidget                                *m_pSidebarAnchorWidget;
    QLabel                                 *m_pPlaceholderLabel;
    QPlainTextEdit                         *m_pPlainTextEdit;
    DSLRLabView                            *m_pDSLRLabView;
    QSlider                                *m_pSlider;
    QSplitter                              *m_pVSplitter;

    // Actions
    QActionGroup                           *m_pViewActionGroup;
    //QAction            *m_pFileOpenAction;
    QAction                                *m_pActionFileOpen;
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

    void openFile(void);
    void createActions(void);
    void createMenus(void);
    void updateUI(bool);
};

#endif // MAINWINDOW_H
