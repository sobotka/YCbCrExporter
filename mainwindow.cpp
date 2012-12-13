#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ffsequence.h"

#include <QDesktopWidget>
#include <stdexcept>
#include <QFileDialog>
#include <QMessageBox>
#include <QtCore>

//***********************
// * CustomApplication
// **********************
CustomApplication::CustomApplication(int &argc, char **argv)  :
    QApplication(argc, argv)
{
}

CustomApplication::~CustomApplication()
{

}

bool CustomApplication::notify(QObject *receiver, QEvent *event)
{
    try
    {
        return QApplication::notify(receiver, event);
    }
    catch(ffError eff)
    {

    }
    catch(std::exception& e)
    {
        QMessageBox msgBox(activeWindow());

        msgBox.setText(e.what());
        msgBox.setInformativeText(tr("This should have been caught. Report a"
                                     "bug with the above information please."));
        msgBox.setStandardButtons(QMessageBox::Close);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowModality(Qt::WindowModal);

        msgBox.exec();
    }
    return false;
}

//***********************
// * MainWindow
// **********************
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setGeometry(QStyle::alignedRect(
                    Qt::LeftToRight, Qt::AlignCenter,
                    QSize(DSLRLAB_DEFAULT_WIDTH,
                          DSLRLAB_DEFAULT_HEIGHT),
                    QApplication::desktop()->availableGeometry()));

    // Provide an empty anchoring widget to anchor the QVBoxLayout.
    m_pMainAnchorWidget = new QWidget;
    m_pSidebarAnchorWidget = new QWidget;

    m_pPlaceholderLabel = new QLabel(tr("Placeholder!!!"));

    setCentralWidget(m_pMainAnchorWidget);

    // Main top to bottom layout.
    m_pVBoxLayout = new QVBoxLayout;
    centralWidget()->setLayout(m_pVBoxLayout);

    // Sidebar top to bottom layout.
    m_pSidebarLayout = new QVBoxLayout;
    m_pPlainTextEdit = new QPlainTextEdit;

    QPalette tempPal = m_pPlainTextEdit->palette();
    tempPal.setColor(QPalette::Base, tempPal.color(QPalette::Dark));
    m_pPlainTextEdit->setPalette(tempPal);
    m_pPlainTextEdit->setReadOnly(true);
    //m_pPlainTextEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    m_pSidebarAnchorWidget->setLayout(m_pSidebarLayout);
    m_pSidebarLayout->addWidget(m_pPlaceholderLabel);
    m_pSidebarLayout->setMargin(DSLRLAB_NOMARGIN);
    m_pSidebarLayout->addWidget(m_pPlainTextEdit);

    m_pVSplitter = new QSplitter(Qt::Horizontal);
    m_pDSLRLabView = new DSLRLabView;

    m_pVSplitter->addWidget(m_pSidebarAnchorWidget);
    m_pVSplitter->addWidget(m_pDSLRLabView);

    //Establish the scaling ratios initially.
    m_pVSplitter->setStretchFactor(m_pVSplitter->indexOf(m_pSidebarAnchorWidget),
                                   DSLRLAB_SIDEBAR_RATIO);
    m_pVSplitter->setStretchFactor(m_pVSplitter->indexOf(m_pDSLRLabView),
                                   DSLRLAB_PRIMARYVIEW_RATIO);

    m_pSlider = new QSlider(Qt::Horizontal);

    m_pVBoxLayout->addWidget(m_pVSplitter);
    m_pVBoxLayout->addWidget(m_pSlider);


    createActions();
    createMenus();

    updateUI(m_pDSLRLabView->isValidSequence());
    LOG_MSG("Logging started...");
}

MainWindow::~MainWindow()
{
}

void MainWindow::openFile(void)
{
    QFileDialog::Options    options;
    QString                 selectedFilter;
    QString                 fileName =
                                QFileDialog::getOpenFileName(this,
                                tr("QFileDialog::getOpenFileName()"),
                                this->windowTitle(),
                                tr("MOV Files (*.MOV);;All Files (*)"),
                                &selectedFilter,
                                options);

    QFuture<void> future = QtConcurrent::run(this, &MainWindow::actionOpenFile, fileName);
}

void MainWindow::updateUI(bool isValid)
{
    if (isValid)
    {
        m_pSlider->setMinimum(FF_FIRST_FRAME);
        m_pSlider->setMaximum(m_pDSLRLabView->getTotalFrames());
        m_pSlider->setValue(FF_FIRST_FRAME);
        m_pSlider->setEnabled(true);
        m_pMenuView->setEnabled(true);
        m_pDSLRLabView->getGraphicsPixmapItem()->setVisible(true);
    }
    else
    {
        m_pSlider->setMinimum(0);
        m_pSlider->setValue(0);
        m_pSlider->setMaximum(0);
        m_pSlider->setEnabled(false);
        m_pMenuView->setEnabled(false);
        m_pDSLRLabView->getGraphicsPixmapItem()->setVisible(false);
    }
}

void MainWindow::createActions(void)
{
    DSLRLabView::connect(m_pDSLRLabView, SIGNAL(signal_frameChanged(long)),
                         this, SLOT(actionFrameChange(long)));

    DSLRLabView::connect(m_pDSLRLabView, SIGNAL(signal_sequenceNew()),
                         this, SLOT(actionSequenceNew()));

    // File Menu Actions
    m_pActionFileOpen = new QAction(tr("&Open"), this);
    m_pActionFileOpen->setShortcut(tr("Ctrl+o"));
    connect(m_pActionFileOpen, SIGNAL(triggered()), this,
            SLOT(actionMenuFileOpen()));

    m_pActionFileQuit = new QAction(tr("&Quit"), this);
    m_pActionFileQuit->setShortcut(tr("Ctrl+q"));
    connect(m_pActionFileQuit, SIGNAL(triggered()), this,
            SLOT(actionMenuFileQuit()));

    // View Menu Actions
    m_pActionViewFitToView = new QAction(tr("&Fit to View"), this);
    m_pActionViewFitToView->setShortcut(tr("`"));
    connect(m_pActionViewFitToView, SIGNAL(triggered()), this,
            SLOT(actionMenuViewFitToView()));

    m_pActionViewZoom1x = new QAction(tr("Zoom &1x"), this);
    m_pActionViewZoom1x->setShortcut(tr("1"));
    connect(m_pActionViewZoom1x, SIGNAL(triggered()), this,
            SLOT(actionMenuViewZoom1x()));

    m_pActionViewZoom2x = new QAction(tr("Zoom in by &2x"), this);
    m_pActionViewZoom2x->setShortcut(tr("2"));
    connect(m_pActionViewZoom2x, SIGNAL(triggered()), this,
            SLOT(actionMenuViewZoom2x()));

    m_pActionViewZoom4x = new QAction(tr("Zoom in by &4x"), this);
    m_pActionViewZoom4x->setShortcut(tr("4"));
    connect(m_pActionViewZoom4x, SIGNAL(triggered()), this,
            SLOT(actionMenuViewZoom4x()));

    // Frame Change Actions
    connect(m_pSlider, SIGNAL(valueChanged(int)), this,
            SLOT(actionFrameChange(int)));
}

void MainWindow::createMenus(void)
{
    // File Menu
    m_pMenuFile = new QMenu(tr("&File"), this);
    m_pMenuFile->addAction(m_pActionFileOpen);
    m_pMenuFile->addSeparator();
    m_pMenuFile->addAction(m_pActionFileQuit);
    menuBar()->addMenu(m_pMenuFile);

    // View Menu
    m_pMenuView = new QMenu(tr("&View"), this);
    m_pMenuView->addAction(m_pActionViewZoom1x);
    m_pMenuView->addAction(m_pActionViewZoom2x);
    m_pMenuView->addAction(m_pActionViewZoom4x);
    m_pMenuView->addAction(m_pActionViewFitToView);
    menuBar()->addMenu(m_pMenuView);
}

/**
 * MainWindow Actions
 **/
void MainWindow::actionMenuFileOpen()
{
    try
    {
        openFile();
    }
    catch (std::exception e)
    {
        m_pSlider->setEnabled(false);
        throw;
    }
}

void MainWindow::actionMenuFileQuit()
{
    QApplication::exit();
}

void MainWindow::actionMenuViewFitToView()
{
    m_pDSLRLabView->fitToView();
}

void MainWindow::actionMenuViewZoom1x()
{
    m_pDSLRLabView->resetTransform();
}

void MainWindow::actionMenuViewZoom2x()
{
    if (m_pDSLRLabView->underMouse())
        m_pDSLRLabView->setScale(2.0,
                                 m_pDSLRLabView->mapFromGlobal(QCursor::pos()));
    else
        m_pDSLRLabView->setScale(2.0, m_pDSLRLabView->getCenter().toPoint());
}

void MainWindow::actionMenuViewZoom4x()
{
    if (m_pDSLRLabView->underMouse())
        m_pDSLRLabView->setScale(4.0,
                                 m_pDSLRLabView->mapFromGlobal(QCursor::pos()));
    else
        m_pDSLRLabView->setScale(4.0, m_pDSLRLabView->getCenter().toPoint());
}

// Signals and slots must match 1:1 with variable declarations. Frames
// are long by default, so we must create an additional function for
// the int passed from the slider.
void MainWindow::actionFrameChange(int frame)
{
    if (m_pDSLRLabView->isValidSequence())
        m_pDSLRLabView->updateCurrentFrame(frame);
}

void MainWindow::actionFrameChange(long frame)
{
    if (m_pDSLRLabView->isValidSequence())
        m_pDSLRLabView->updateCurrentFrame(frame);
}

void MainWindow::actionSequenceNew()
{
    m_pDSLRLabView->sequenceNew();
    updateUI(m_pDSLRLabView->isValidSequence());
}

void MainWindow::actionOpenFile(QString fileName)
{
    try
    {
        if (fileName.isNull())
            throw ffError("fileName.isNull()", FFERROR_BAD_FILENAME);
        m_pDSLRLabView->openFile(fileName.toUtf8().data());

    }
    catch (ffError eff)
    {
        // To simplify our code above this call so that we can avoid many if
        // else situations, simply rethrow the exception and take no
        // extraordinary action when the filename is NULL, indicating a cancel.
        if (eff.getError() == FFERROR_BAD_FILENAME)
            throw;
        else
        {
            QMessageBox msgBox(this);

            QString errorMessage =
                    tr("There was an error loading the video file specified. "
                       "Please confirm that the file type is supported. "
                       "Error: (") + QString(eff.what()) + ")";
            msgBox.setText(tr("Error loading file"));
            msgBox.setInformativeText(errorMessage);
            msgBox.setStandardButtons(QMessageBox::Close);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowModality(Qt::WindowModal);

            msgBox.exec();
        }
    }
}
