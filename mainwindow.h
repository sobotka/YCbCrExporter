#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <dslrlabview.h>
#include <ffsequence.h>

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QSlider>
#include <QAction>
#include <QMenu>

#define DSLRLAB_DEFAULT_WIDTH               1000
#define DSLRLAB_DEFAULT_HEIGHT              562

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

private:
    // Widgets
    QWidget                                *m_pAnchorWidget;
    QVBoxLayout                            *m_pVBoxLayout;
    DSLRLabView                            *m_pDSLRLabView;
    QSlider                                *m_pSlider;
    // Actions
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
