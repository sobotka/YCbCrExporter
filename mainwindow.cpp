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
    case (ffSequence::isValid):
        break;
    case (ffSequence::justClosed):
    case (ffSequence::justErrored):
    case (ffSequence::isInvalid):
        m_pViewActionGroup->setDisabled(true);
        m_pActionFileOpen->setEnabled(true);
        m_pActionFileSave->setDisabled(true);
        m_pMenuView->setEnabled(false);
        break;
    case (ffSequence::isLoading):
    case (ffSequence::justLoading):
        m_pViewActionGroup->setDisabled(true);
        m_pActionFileOpen->setDisabled(true);
        m_pActionFileSave->setDisabled(true);
        m_pMenuView->setEnabled(false);
        break;
    case (ffSequence::justOpened):
        m_pViewActionGroup->setEnabled(true);
        m_pActionFileOpen->setEnabled(true);
        m_pActionFileSave->setEnabled(true);
        m_pMenuView->setEnabled(true);
        break;
    }
}

void MainWindow::createObjects(void)
{
    // Provide an empty anchoring widget to anchor the QVBoxLayout.
    m_pMainAnchorWidget = new QWidget;
    m_pMainAnchorWidget->setStyleSheet("background:lightGray");

    m_pOutputOptionsAnchor = new QWidget;
    m_pDisplayOptionsAnchor = new QWidget;

    m_pDisplayPlaneLabel = new QLabel(tr("Viewer Planes:"));
    m_pDisplayPlaneCombo = new QComboBox;

    // Main top to bottom layout.
    m_pVBoxLayout = new QVBoxLayout;

    // Sidebar top to bottom layout.
    m_pOutputOptionsLayout = new QFormLayout;
    m_pDisplayOptionsLayout = new QBoxLayout(QBoxLayout::TopToBottom);

    m_pSplitter = new QSplitter(Qt::Horizontal);
    m_pSidebarToolBox = new QToolBox;

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
    centralWidget()->setLayout(m_pVBoxLayout);

    initSidebar();
    m_pSplitter->addWidget(m_pSidebarToolBox);
    m_pSplitter->addWidget(m_pDSLRLabView);

    //Establish the scaling ratios initially.
    m_pSplitter->setStretchFactor(m_pSplitter->indexOf(m_pSidebarToolBox),
                                   DSLRLAB_SIDEBAR_RATIO);
    m_pSplitter->setStretchFactor(m_pSplitter->indexOf(m_pDSLRLabView),
                                   DSLRLAB_PRIMARYVIEW_RATIO);

    m_pVBoxLayout->addWidget(m_pSplitter);
}

void MainWindow::initSidebar(void)
{
    m_pDisplayPlaneCombo->addItem(tr("Y"), ffRawFrame::Y);
    m_pDisplayPlaneCombo->addItem(tr("Cb"), ffRawFrame::Cb);
    m_pDisplayPlaneCombo->addItem(tr("Cr"), ffRawFrame::Cr);
    m_pDisplayPlaneCombo->addItem(tr("Combined"), ffRawFrame::Combined);
    m_pDisplayPlaneCombo->setCurrentIndex(m_pDSLRLabView->getDisplayPlane());

    m_pOutputOptionsAnchor->setLayout(m_pOutputOptionsLayout);
    m_pOutputOptionsLayout->setMargin(DSLRLAB_NOMARGIN);
    m_pDisplayOptionsAnchor->setLayout(m_pDisplayOptionsLayout);
    m_pDisplayOptionsLayout->setMargin(DSLRLAB_NOMARGIN);

    m_pSidebarToolBox->addItem(m_pOutputOptionsAnchor, tr("Output"));
    m_pSidebarToolBox->addItem(m_pDisplayOptionsAnchor, tr("Display"));
    m_pSidebarToolBox->setCurrentIndex(1);

    m_pDisplayOptionsLayout->addWidget(m_pDisplayPlaneCombo);
    m_pDisplayOptionsLayout->addStretch();

    m_pSidebarToolBox->setMinimumWidth(SIDEBAR_MINIMUM_WIDTH);
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

    m_pActionFileSave = new QAction(tr("&Save..."), this);
    m_pActionFileSave->setShortcut(tr("Ctrl+s"));
    connect(m_pActionFileSave, SIGNAL(triggered()), this,
            SLOT(onMenuFileSave()));
    m_pFileActionGroup->addAction(m_pActionFileSave);

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
            SLOT(onError(QString)));
    connect(m_pDSLRLabView, SIGNAL(signal_updateUI(ffSequence::ffSequenceState)), this,
            SLOT(onUpdateUI(ffSequence::ffSequenceState)));
    connect(m_pDisplayPlaneCombo, SIGNAL(currentIndexChanged(int)), this,
            SLOT(onDisplayPlaneChange(int)));
}

void MainWindow::createMenus(void)
{
    // File Menu
    m_pMenuFile = new QMenu(tr("&File"), this);
    m_pMenuFile->addAction(m_pActionFileOpen);
    m_pMenuFile->addAction(m_pActionFileSave);
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

void MainWindow::onMenuFileSave()
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

void MainWindow::onMenuViewZoom2x()
{
    m_pDSLRLabView->getTextPillItem()->start(tr("Zoom 2x"));
}

void MainWindow::onMenuViewZoom4x()
{
    m_pDSLRLabView->getTextPillItem()->start(tr("Zoom 4x"));
}

void MainWindow::onDisplayPlaneChange(int plane)
{
    m_pDSLRLabView->setDisplayPlane((ffRawFrame::PlaneType)plane);
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
