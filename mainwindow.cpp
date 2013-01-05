#include "mainwindow.h"

/******************************************************************************
 * CustomApplication
 ******************************************************************************/
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

/******************************************************************************
 * MainWindow
 ******************************************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    createObjects();
    initObjects();
    createActions();
    createMenus();

    onUpdateUI(m_pDSLRLabView->getState());
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
                                               &MainWindow::onOpenFile,
                                               fileName);
        }
    }
}

void MainWindow::exportFile(void)
{
    if (!m_workerThread.isRunning())
    {
        QFileDialog::Options    options;
        QString                 selectedFilter;
        QFileInfo               fileInfo(m_pDSLRLabView->getFileURI());

        QString                 fileName =
            QFileDialog::getSaveFileName(this,
                                         tr("QFileDialog::getSaveFileName()"),
                                         fileInfo.baseName(),
                                         tr("All Files (*);;JPEG Files (*.JPG)"
                                            ";;OpenEXR Files(*.EXR)"),
                                         &selectedFilter,
                                         options);
        if (!fileName.isNull())
        {
            long start, end;
            m_workerThread = QtConcurrent::run(this,
                                               &MainWindow::onExport,
                                               fileName, start, end);
        }
    }
}

void MainWindow::onUpdateUI(ffSequence::ffSequenceState state)
{
    switch (state)
    {
    case (ffSequence::justClosed):
    case (ffSequence::justErrored):
    case (ffSequence::isInvalid):
        m_pViewActionGroup->setDisabled(true);
        m_pActionFileOpen->setEnabled(true);
        m_pActionFileExport->setDisabled(true);
        m_pMenuView->setDisabled(true);
        m_pSBToolBox->setDisabled(true);
        m_pSBEStartSpin->setMinimum(ffDefault::NoFrame);
        m_pSBEStartSpin->setValue(ffDefault::NoFrame);
        m_pSBEStartSpin->setMaximum(ffDefault::NoFrame);
        m_pSBEEndSpin->setMinimum(ffDefault::NoFrame);
        m_pSBEEndSpin->setValue(ffDefault::NoFrame);
        m_pSBEEndSpin->setMaximum(ffDefault::NoFrame);
        m_pSBToolBox->setEnabled(false);
        break;
    case (ffSequence::isLoading):
    case (ffSequence::justLoading):
        m_pViewActionGroup->setDisabled(true);
        m_pActionFileOpen->setDisabled(true);
        m_pActionFileExport->setDisabled(true);
        m_pMenuView->setDisabled(true);
        m_pSBToolBox->setDisabled(true);
        m_pSBEStartSpin->setMinimum(ffDefault::NoFrame);
        m_pSBEStartSpin->setValue(ffDefault::NoFrame);
        m_pSBEStartSpin->setMaximum(ffDefault::NoFrame);
        m_pSBEEndSpin->setMinimum(ffDefault::NoFrame);
        m_pSBEEndSpin->setValue(ffDefault::NoFrame);
        m_pSBEEndSpin->setMaximum(ffDefault::NoFrame);
        m_pSBToolBox->setEnabled(false);
        break;
    case (ffSequence::isValid):
    case (ffSequence::justOpened):
        m_pViewActionGroup->setEnabled(true);
        m_pActionFileOpen->setEnabled(true);
        m_pActionFileExport->setEnabled(true);
        m_pMenuView->setEnabled(true);
        m_pSBEStartSpin->setMinimum(ffDefault::FirstFrame);
        m_pSBEStartSpin->setMaximum(m_pDSLRLabView->getTotalFrames());
        m_pSBEStartSpin->setValue(ffDefault::FirstFrame);
        m_pSBEEndSpin->setMinimum(ffDefault::FirstFrame);
        m_pSBEEndSpin->setMaximum(m_pDSLRLabView->getTotalFrames());
        m_pSBEEndSpin->setValue(m_pDSLRLabView->getTotalFrames());
        m_pSBToolBox->setEnabled(true);
        break;
    }
}

void MainWindow::createObjects(void)
{
    // Provide an empty anchoring widget to anchor the QVBoxLayout.
    m_pMainAnchorWidget = new QWidget;
    m_pMainAnchorWidget->setStyleSheet("background:lightGray");

    m_pSBEAnchor = new QWidget;
    m_pSBVAnchor = new QWidget;

    // Main top to bottom layout.
    m_pMainLayout = new QVBoxLayout;

    // Sidebar top to bottom layouts.
    m_pSBELayout = new QVBoxLayout;
    m_pSBVLayout = new QVBoxLayout;

    // Export Interface
    m_pSBEPlaneLabel = new QLabel(tr("Plane(s) Exported:"));
    m_pSBEPlaneCombo = new QComboBox;
    m_pSBEStartEndLabel = new QLabel(tr("Start / End Frames:"));
    m_pSBEStartEndLayout = new QHBoxLayout;
    m_pSBEStartSpin = new QSpinBox;
    m_pSBEEndSpin = new QSpinBox;

    // Viewer Interface
    m_pSBVPlaneLabel = new QLabel(tr("Plane(s) Viewed:"));
    m_pSBVPlaneCombo = new QComboBox;

    m_pSplitter = new QSplitter(Qt::Horizontal);
    m_pSBToolBox = new QToolBox;

    m_pDSLRLabView = new DSLRLabView;
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
    centralWidget()->setLayout(m_pMainLayout);

    initSidebar();
    m_pSplitter->addWidget(m_pSBToolBox);
    m_pSplitter->addWidget(m_pDSLRLabView);

    m_pMainLayout->addWidget(m_pSplitter);
}

void MainWindow::initSidebar(void)
{
    // Export Plane Combo Initialization
    m_pSBEPlaneCombo->addItem(tr("Combined RGB"), ffExportDetails::RGB);
    m_pSBEPlaneCombo->addItem(tr("YCbCr"), ffExportDetails::YCbCr);
    m_pSBEPlaneCombo->addItem(tr("Raw Unscaled YCbCr"), ffExportDetails::Raw);
    m_pSBEPlaneCombo->setCurrentIndex(ffExportDetails::RGB);

    // Viewer Plane Combo Initialization
    m_pSBVPlaneCombo->addItem(tr("Y"), ffViewer::Y);
    m_pSBVPlaneCombo->addItem(tr("Cb"), ffViewer::Cb);
    m_pSBVPlaneCombo->addItem(tr("Cr"), ffViewer::Cr);
    m_pSBVPlaneCombo->addItem(tr("Combined RGB"), ffViewer::RGB);
    m_pSBVPlaneCombo->setCurrentIndex(ffViewer::Y);

    m_pSBEAnchor->setLayout(m_pSBELayout);
    m_pSBELayout->setMargin(NOMARGIN);
    m_pSBVAnchor->setLayout(m_pSBVLayout);
    m_pSBVLayout->setMargin(NOMARGIN);

    m_pSBToolBox->addItem(m_pSBEAnchor, tr("Export"));
    m_pSBToolBox->addItem(m_pSBVAnchor, tr("Viewer"));
    m_pSBToolBox->setCurrentIndex(1);

    // Export Interface

    m_pSBELayout->addWidget(m_pSBEPlaneLabel);
    m_pSBELayout->addWidget(m_pSBEPlaneCombo);

    m_pSBELayout->addWidget(m_pSBEStartEndLabel);
    QWidget *pSBEStartEndAnchor = new QWidget;
    m_pSBELayout->addWidget(pSBEStartEndAnchor);
    m_pSBEStartEndLayout->setMargin(NOMARGIN);
    pSBEStartEndAnchor->setLayout(m_pSBEStartEndLayout);
    m_pSBEStartEndLayout->addWidget(m_pSBEStartSpin);
    m_pSBEStartEndLayout->addWidget(m_pSBEEndSpin);
    m_pSBELayout->addStretch();

    // Viewer Interface
    m_pSBVLayout->addWidget(m_pSBVPlaneLabel);
    m_pSBVLayout->addWidget(m_pSBVPlaneCombo);
    m_pSBVLayout->addStretch();

    m_pSBToolBox->setMinimumWidth(SIDEBAR_MINIMUM_WIDTH);

    connect(m_pSBVPlaneCombo, SIGNAL(currentIndexChanged(int)), this,
            SLOT(onSidebarViewerPlaneChange(int)));
}

void MainWindow::createActions(void)
{
    // File Menu Actions
    m_pFileActionGroup = new QActionGroup(this);

    m_pActionFileOpen = new QAction(tr("&Open..."), this);
    m_pActionFileOpen->setShortcut(tr("Ctrl+o"));
    connect(m_pActionFileOpen, SIGNAL(triggered()), this,
            SLOT(onMenuFileOpen()));
    m_pFileActionGroup->addAction(m_pActionFileOpen);

    m_pActionFileExport = new QAction(tr("&Export"), this);
    m_pActionFileExport->setShortcut(tr("Ctrl+s"));
    connect(m_pActionFileExport, SIGNAL(triggered()), this,
            SLOT(onMenuFileExport()));
    m_pFileActionGroup->addAction(m_pActionFileExport);

    m_pActionFileQuit = new QAction(tr("&Quit"), this);
    m_pActionFileQuit->setShortcut(tr("Ctrl+q"));
    connect(m_pActionFileQuit, SIGNAL(triggered()), this,
            SLOT(onMenuFileQuit()));

    // View Menu Actions
    m_pViewActionGroup = new QActionGroup(this);

    m_pActionViewFitToView = new QAction(tr("&Fit to View"), this);
    m_pActionViewFitToView->setShortcut(tr("`"));
    connect(m_pActionViewFitToView, SIGNAL(triggered()), this,
            SLOT(onMenuViewFitToView()));
    m_pViewActionGroup->addAction(m_pActionViewFitToView);

    m_pActionViewZoom1x = new QAction(tr("Zoom &1x"), this);
    m_pActionViewZoom1x->setShortcut(tr("1"));
    connect(m_pActionViewZoom1x, SIGNAL(triggered()), this,
            SLOT(onMenuViewZoom1x()));
    m_pViewActionGroup->addAction(m_pActionViewZoom1x);

    connect(m_pDSLRLabView, SIGNAL(signal_error(QString)), this,
            SLOT(onError(QString)));
    connect(m_pDSLRLabView, SIGNAL(signal_updateUI(ffSequence::ffSequenceState)), this,
            SLOT(onUpdateUI(ffSequence::ffSequenceState)));
}

void MainWindow::createMenus(void)
{
    // File Menu
    m_pMenuFile = new QMenu(tr("&File"), this);
    m_pMenuFile->addAction(m_pActionFileOpen);
    m_pMenuFile->addAction(m_pActionFileExport);
    m_pMenuFile->addSeparator();
    m_pMenuFile->addAction(m_pActionFileQuit);
    menuBar()->addMenu(m_pMenuFile);

    // View Menu
    m_pMenuView = new QMenu(tr("&View"), this);
    m_pMenuView->addAction(m_pActionViewZoom1x);
    m_pMenuView->addAction(m_pActionViewFitToView);
    menuBar()->addMenu(m_pMenuView);
}

/******************************************************************************
 * Event Overrides
 ******************************************************************************/
void MainWindow::onMenuFileOpen()
{
    try
    {
        openFile();
    }
    catch (std::exception e)
    {
        onUpdateUI(ffSequence::justErrored);
        throw;
    }
}

void MainWindow::onMenuFileExport()
{
    try
    {
        exportFile();
    }
    catch (ffError eff)
    {
        onUpdateUI(ffSequence::justErrored);
    }
    catch (std::exception e)
    {
        onUpdateUI(ffSequence::justErrored);
        throw;
    }
}

void MainWindow::onMenuFileQuit()
{
    QApplication::exit();
}

void MainWindow::onMenuViewFitToView()
{
    m_pDSLRLabView->fitToView();
    m_pDSLRLabView->getTextPillItem()->start(tr("Fit to View"));
}

void MainWindow::onMenuViewZoom1x()
{
    m_pDSLRLabView->resetTransform();
    m_pDSLRLabView->getTextPillItem()->start(tr("Zoom 1x"));
}

void MainWindow::onSidebarViewerPlaneChange(int plane)
{
    m_pDSLRLabView->setViewerPlane((ffViewer::ViewerPlane)plane);
}

void MainWindow::onOpenFile(QString fileName)
{
    try
    {
        if (fileName.isNull())
            throw ffError("fileName.isNull()", ffError::ERROR_NULL_FILENAME);
        m_pDSLRLabView->openSequence(fileName.toUtf8().data());
    }
    catch (ffError eff)
    {
        /*
         * The only legitimate case to get here is in the instance of a
         * null filename, at which point we simply pass on it. The other
         * more important errors should be caught at the lower level and
         * have an emit generated to be received by the onError handler.
         */
    }
}

void MainWindow::onExport(QString fileName, long start, long end)
{
    try
    {
        if (fileName.isNull())
            throw ffError("fileName.isNull()", ffError::ERROR_NULL_FILENAME);
        m_pDSLRLabView->saveSequence(fileName.toUtf8().data(), start, end);
    }
    catch (ffError eff)
    {
        /*
         * The only legitimate case to get here is in the instance of a
         * null filename, at which point we simply pass on it. The other
         * more important errors should be caught at the lower level and
         * have an emit generated to be received by the onError handler.
         */
    }
}

void MainWindow::onError(QString message)
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

    onUpdateUI(m_pDSLRLabView->getState());
}
