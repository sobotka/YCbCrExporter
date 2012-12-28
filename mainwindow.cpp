#include "mainwindow.h"
#include "ffsequence.h"

#include <QDesktopWidget>
#include <stdexcept>
#include <QFileDialog>
#include <QMessageBox>
#include <QtCore>
#include <QtGui>

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
        if (m_pMainWindow)
        {
            QString message("An unexpected error has occurred. Please "
                    "report the issue at GitHub with <" + QString(e.what()) + "> included "
                    "and relevant information.");
            QMessageBox::critical(m_pMainWindow, e.what(), message);
        }
    }
    return false;
}

//***********************
// * MainWindow
// **********************
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    createObjects();
    initObjects();
    createActions();
    createMenus();

    updateUI(m_pDSLRLabView->getState());
    LOG_MSG("Logging started...");
}

MainWindow::~MainWindow()
{
    // Hackish but simple way to avoid the threading issue where an artist
    // loads a file and shuts down the window prior to it being loaded.
    // May have to migrate to a mutex for more sophisticated processing.
    while (m_workerThread.isRunning());
}

void MainWindow::openFile(void)
{
    if (!m_workerThread.isRunning())
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
        if (!fileName.isNull())
        {
            m_workerThread = QtConcurrent::run(this,
                                               &MainWindow::actionOpenFile,
                                               fileName);
        }
    }
}

void MainWindow::updateUI(ffSequence::ffSequenceState state)
{
    switch (state)
    {
    case (ffSequence::isValid):
        connect(m_pSlider, SIGNAL(valueChanged(int)), this,
                SLOT(actionFrameChange(int)));
        m_pViewActionGroup->setEnabled(true);
        m_pSlider->setMinimum(FF_FIRST_FRAME);
        m_pSlider->setMaximum(m_pDSLRLabView->getTotalFrames());
        m_pSlider->setValue(FF_FIRST_FRAME);
        m_pSlider->setEnabled(true);
        m_pMenuView->setEnabled(true);
        m_pActionFileOpen->setEnabled(true);
        m_pSlider->show();
        break;
    case (ffSequence::isInvalid):
        disconnect(m_pSlider, SIGNAL(valueChanged(int)), this,
                   SLOT(actionFrameChange(int)));
        m_pViewActionGroup->setDisabled(true);
        m_pSlider->setMinimum(0);
        m_pSlider->setValue(0);
        m_pSlider->setMaximum(0);
        m_pSlider->setEnabled(false);
        m_pMenuView->setEnabled(false);
        m_pActionFileOpen->setEnabled(true);
        m_pSlider->hide();
        break;
    case (ffSequence::isLoading):
        disconnect(m_pSlider, SIGNAL(valueChanged(int)), this,
                   SLOT(actionFrameChange(int)));
        m_pViewActionGroup->setDisabled(true);
        m_pSlider->setMinimum(0);
        m_pSlider->setValue(0);
        m_pSlider->setMaximum(0);
        m_pSlider->setEnabled(false);
        m_pMenuView->setEnabled(false);
        m_pActionFileOpen->setEnabled(false);
        m_pSlider->hide();
        break;
    }
}

void MainWindow::createObjects(void)
{
    // Provide an empty anchoring widget to anchor the QVBoxLayout.
    m_pMainAnchorWidget = new QWidget;
    m_pSidebarAnchorWidget = new QWidget;

    m_pDisplayPlaneLabel = new QLabel(tr("Display Plane:"));
    m_pDisplayPlaneCombo = new QComboBox;

    // Main top to bottom layout.
    m_pVBoxLayout = new QVBoxLayout;

    // Sidebar top to bottom layout.
    m_pSidebarLayout = new QVBoxLayout;
    m_pPlainTextEdit = new QPlainTextEdit;

    m_pVSplitter = new QSplitter(Qt::Horizontal);
    m_pDSLRLabView = new DSLRLabView;

    m_pSlider = new QSlider(Qt::Horizontal);
}

void MainWindow::initObjects(void)
{
    setGeometry(QStyle::alignedRect(
                    Qt::LeftToRight, Qt::AlignCenter,
                    QSize(DSLRLAB_DEFAULT_WIDTH,
                          DSLRLAB_DEFAULT_HEIGHT),
                    QApplication::desktop()->availableGeometry()));

    setWindowTitle(tr("YCbCr Lab"));

    setCentralWidget(m_pMainAnchorWidget);
    centralWidget()->setLayout(m_pVBoxLayout);

    m_pDisplayPlaneCombo->addItem(tr("Y"), ffRawFrame::Y);
    m_pDisplayPlaneCombo->addItem(tr("Cb"), ffRawFrame::Cb);
    m_pDisplayPlaneCombo->addItem(tr("Cr"), ffRawFrame::Cr);
    m_pDisplayPlaneCombo->addItem(tr("Combined"), ffRawFrame::Combined);
    m_pDisplayPlaneCombo->setCurrentIndex(m_pDSLRLabView->getDisplayPlane());

    m_pSidebarAnchorWidget->setLayout(m_pSidebarLayout);
    m_pSidebarLayout->setMargin(DSLRLAB_NOMARGIN);
    m_pSidebarLayout->addWidget(m_pDisplayPlaneLabel);
    m_pSidebarLayout->addWidget(m_pDisplayPlaneCombo);
    m_pSidebarLayout->addWidget(m_pPlainTextEdit);

    m_pVSplitter->addWidget(m_pSidebarAnchorWidget);
    m_pVSplitter->addWidget(m_pDSLRLabView);

    //Establish the scaling ratios initially.
    m_pVSplitter->setStretchFactor(m_pVSplitter->indexOf(m_pSidebarAnchorWidget),
                                   DSLRLAB_SIDEBAR_RATIO);
    m_pVSplitter->setStretchFactor(m_pVSplitter->indexOf(m_pDSLRLabView),
                                   DSLRLAB_PRIMARYVIEW_RATIO);

    m_pVBoxLayout->addWidget(m_pVSplitter);
    m_pVBoxLayout->addWidget(m_pSlider);

    QPalette tempPal = m_pPlainTextEdit->palette();
    tempPal.setColor(QPalette::Base, tempPal.color(QPalette::Dark));
    m_pPlainTextEdit->setPalette(tempPal);
    m_pPlainTextEdit->setReadOnly(true);
}

void MainWindow::createActions(void)
{
    DSLRLabView::connect(m_pDSLRLabView, SIGNAL(signal_frameChanged(long)),
                         this, SLOT(actionFrameChange(long)));

    // File Menu Actions
    m_pActionFileOpen = new QAction(tr("&Open"), this);
    m_pActionFileOpen->setShortcut(tr("Ctrl+o"));
    connect(m_pActionFileOpen, SIGNAL(triggered()), this,
            SLOT(actionMenuFileOpen()));

    m_pActionFileQuit = new QAction(tr("&Quit"), this);
    m_pActionFileQuit->setShortcut(tr("Ctrl+q"));
    connect(m_pActionFileQuit, SIGNAL(triggered()), this,
            SLOT(actionMenuFileQuit()));

    m_pViewActionGroup = new QActionGroup(this);

    // View Menu Actions
    m_pActionViewFitToView = new QAction(tr("&Fit to View"), this);
    m_pActionViewFitToView->setShortcut(tr("`"));
    connect(m_pActionViewFitToView, SIGNAL(triggered()), this,
            SLOT(actionMenuViewFitToView()));
    m_pViewActionGroup->addAction(m_pActionViewFitToView);

    m_pActionViewZoom1x = new QAction(tr("Zoom &1x"), this);
    m_pActionViewZoom1x->setShortcut(tr("1"));
    connect(m_pActionViewZoom1x, SIGNAL(triggered()), this,
            SLOT(actionMenuViewZoom1x()));
    m_pViewActionGroup->addAction(m_pActionViewZoom1x);

    /* TODO: Need to figure out the most useful zoom keys. Originally
     * speculated that having hard 2x and 4x might be useful, but perhaps
     * modifiers for the degree of zoom would be wiser.
     *
    m_pActionViewZoom2x = new QAction(tr("Zoom in by &2x"), this);
    m_pActionViewZoom2x->setShortcut(tr("2"));
    connect(m_pActionViewZoom2x, SIGNAL(triggered()), this,
            SLOT(actionMenuViewZoom2x()));
    m_pViewActionGroup->addAction(m_pActionViewZoom2x);

    m_pActionViewZoom4x = new QAction(tr("Zoom in by &4x"), this);
    m_pActionViewZoom4x->setShortcut(tr("4"));
    connect(m_pActionViewZoom4x, SIGNAL(triggered()), this,
            SLOT(actionMenuViewZoom4x()));
    m_pViewActionGroup->addAction(m_pActionViewZoom4x); */

    connect(m_pDSLRLabView, SIGNAL(signal_error(QString)), this,
            SLOT(actionError(QString)));
    connect(m_pDSLRLabView, SIGNAL(signal_sequenceNew()), this,
            SLOT(onSequenceNew()));
    connect(m_pDSLRLabView, SIGNAL(signal_sequenceClose()), this,
            SLOT(onSequenceClose()));
    connect(m_pDSLRLabView, SIGNAL(signal_sequenceStartOpen()), this,
            SLOT(onSequenceStartOpen()));
    connect(m_pDisplayPlaneCombo, SIGNAL(currentIndexChanged(int)), this,
            SLOT(onDisplayPlaneChange(int)));
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
    //m_pMenuView->addAction(m_pActionViewZoom2x);
    //m_pMenuView->addAction(m_pActionViewZoom4x);
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
        updateUI(m_pDSLRLabView->getState());
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
    m_pDSLRLabView->getTextPillItem()->start(tr("Fit to View"));
}

void MainWindow::actionMenuViewZoom1x()
{
    m_pDSLRLabView->resetTransform();
    m_pDSLRLabView->getTextPillItem()->start(tr("Zoom 1x"));
}

void MainWindow::actionMenuViewZoom2x()
{
    m_pDSLRLabView->getTextPillItem()->start(tr("Zoom 2x"));
}

void MainWindow::actionMenuViewZoom4x()
{
    m_pDSLRLabView->getTextPillItem()->start(tr("Zoom 4x"));
}

// Signals and slots must match 1:1 with variable declarations. Frames
// are long by default, so we must create an additional function for
// the int passed from the slider.
void MainWindow::actionFrameChange(int frame)
{
    if (m_pDSLRLabView->getState())
        m_pDSLRLabView->updateCurrentFrame(frame);
}

void MainWindow::actionFrameChange(long frame)
{
    if (m_pDSLRLabView->getState() == ffSequence::isValid)
        m_pDSLRLabView->updateCurrentFrame(frame);
}

void MainWindow::onSequenceNew()
{
    updateUI(m_pDSLRLabView->getState());
}

void MainWindow::onSequenceClose()
{
    updateUI(m_pDSLRLabView->getState());
}

void MainWindow::onSequenceStartOpen()
{
    updateUI(ffSequence::isLoading);
}

void MainWindow::onDisplayPlaneChange(int plane)
{
    m_pDSLRLabView->setDisplayPlane((ffRawFrame::PlaneType)plane);
}

void MainWindow::actionOpenFile(QString fileName)
{
    try
    {
        if (fileName.isNull())
            throw ffError("fileName.isNull()", FFERROR_BAD_FILENAME);
        m_pDSLRLabView->openSequence(fileName.toUtf8().data());
    }
    catch (ffError eff)
    {
        /* We should never get here after the emit based refactor.
         * leaving in place in the event that this estimation is
         * wrong. Needs testing.
         *
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
        } */
    }
}

void MainWindow::actionError(QString message)
{
    QMessageBox msgBox(this);

    QString errorMessage =
            tr("There was an error loading the video file specified. "
               "Please confirm that the file type is supported. "
               "Error: <<") + message + ">>";
    msgBox.setText(tr("Error loading file"));
    msgBox.setInformativeText(errorMessage);
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowModality(Qt::WindowModal);

    msgBox.exec();

    updateUI(m_pDSLRLabView->getState());
}
