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

    onStateChanged(m_pDSLRLabView->getState());
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

void MainWindow::createObjects(void)
{
    // Export Interface
    m_pSBEPlaneCombo = new QComboBox;
    m_pSBEInSpin = new QSpinBox;
    m_pSBEOutSpin = new QSpinBox;
    m_pSBEInReset = new QPushButton(tr("Reset In"));
    m_pSBEOutReset= new QPushButton(tr("Reset Out"));

    // Viewer Interface
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

    QWidget *pAnchor = new QWidget;
    setCentralWidget(pAnchor);

    QVBoxLayout *pRows = new QVBoxLayout;
    centralWidget()->setLayout(pRows);

    initSidebar();
    m_pSplitter->addWidget(m_pSBToolBox);
    m_pSplitter->addWidget(m_pDSLRLabView);

    pRows->addWidget(m_pSplitter);
}

void MainWindow::initSidebar(void)
{
    m_pSBToolBox->setMinimumWidth(SIDEBAR_MINIMUM_WIDTH);

    QVBoxLayout *pRows = NULL;
    QVBoxLayout *pSubRows = NULL;
    QHBoxLayout *pSubCols = NULL;
    // Export Interface

    // Export Plane Combo Initialization
    m_pSBEPlaneCombo->addItem(tr("Combined RGB"), ffExportDetails::RGB);
    m_pSBEPlaneCombo->addItem(tr("YCbCr"), ffExportDetails::YCbCr);
    m_pSBEPlaneCombo->addItem(tr("Raw Unscaled YCbCr"), ffExportDetails::Raw);
    m_pSBEPlaneCombo->setCurrentIndex(ffExportDetails::RGB);

    QWidget *pAnchor = new QWidget;
    pRows = new QVBoxLayout;
    //pRows->setMargin(NOMARGIN);
    pAnchor->setLayout(pRows);

    m_pSBToolBox->addItem(pAnchor, tr("Export"));

    // Export Planes
    QGroupBox *pGroupBox = new QGroupBox("Export Planes");
    pRows->addWidget(pGroupBox);
    pSubRows = new QVBoxLayout;
    pGroupBox->setLayout(pSubRows);
    pSubRows->addWidget(m_pSBEPlaneCombo);

    // In / Out Planes
    pGroupBox = new QGroupBox("In / Out Frames");
    pRows->addWidget(pGroupBox);
    pSubRows = new QVBoxLayout;
    pGroupBox->setLayout(pSubRows);
    pSubCols = new QHBoxLayout;
    pSubRows->addLayout(pSubCols);
    pSubCols->addWidget(m_pSBEInSpin);
    pSubCols->addWidget(m_pSBEOutSpin);
    pSubCols = new QHBoxLayout;
    pSubRows->addLayout(pSubCols);
    pSubCols->addWidget(m_pSBEInReset);
    pSubCols->addWidget(m_pSBEOutReset);

    pRows->addStretch();

    // Viewer Interface

    // Viewer Plane Combo Initialization
    m_pSBVPlaneCombo->addItem(tr("Y"), ffViewer::Y);
    m_pSBVPlaneCombo->addItem(tr("Cb"), ffViewer::Cb);
    m_pSBVPlaneCombo->addItem(tr("Cr"), ffViewer::Cr);
    m_pSBVPlaneCombo->addItem(tr("Combined RGB"), ffViewer::RGB);
    m_pSBVPlaneCombo->setCurrentIndex(ffViewer::Y);

    pAnchor = new QWidget;
    pRows = new QVBoxLayout;
    //pRows->setMargin(NOMARGIN);
    pAnchor->setLayout(pRows);
    m_pSBToolBox->addItem(pAnchor, tr("Viewer"));
    pGroupBox = new QGroupBox(tr("Viewer Plane(s)"));
    pRows->addWidget(pGroupBox);
    pSubRows = new QVBoxLayout;
    //pSubRows->setMargin(NOMARGIN);
    pGroupBox->setLayout(pSubRows);
    pSubRows->addWidget(m_pSBVPlaneCombo);
    pRows->addStretch();

    m_pSBToolBox->setCurrentIndex(1);

    connect(m_pSBVPlaneCombo, SIGNAL(currentIndexChanged(int)), this,
            SLOT(onSidebarViewerPlaneChanged(int)));
    connect(m_pSBEInReset, SIGNAL(clicked()), this, SLOT(onSidebarResetIn()));
    connect(m_pSBEOutReset, SIGNAL(clicked()), this, SLOT(onSidebarResetOut()));
    connect(m_pSBEInSpin, SIGNAL(valueChanged(int)), this,
            SLOT(onSidebarSetIn(int)));
    connect(m_pSBEOutSpin, SIGNAL(valueChanged(int)), this,
            SLOT(onSidebarSetOut(int)));
    connect(m_pSBEPlaneCombo, SIGNAL(currentIndexChanged(int)), this,
            SLOT(onSidebarExportPlaneChanged(int)));
    connect(m_pDSLRLabView->getQffSequence(),
            SIGNAL(signal_exportTrimChanged(long,long,void*)), this,
            SLOT(onTrimChanged(long,long,void*)));
    connect(m_pSBEInReset, SIGNAL(clicked()), this, SLOT(onSidebarResetIn()));
    connect(m_pSBEOutReset, SIGNAL(clicked()), this, SLOT(onSidebarResetOut()));
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

    // General Actions
//    m_pActionSetIn  = new QAction(tr("Set &in point"), this);
//    m_pActionSetIn->setShortcut(tr("i"));
//    connect(m_pActionSetIn, SIGNAL(triggered()), this, SLOT(onSidebarSetIn()));

//    m_pActionSetOut = new QAction(tr("Set &out point"), this);
//    m_pActionSetOut->setShortcut(tr("o"));
//    connect(m_pActionSetOut, SIGNAL(triggered()), this, SLOT(onSidebarSetOut()));

//    m_pActionResetIn = new QAction(tr("Reset in point"), this);
//    m_pActionResetIn->setShortcut(tr("u"));
//    connect(m_pActionResetIn, SIGNAL(triggered()), this, SLOT(onSidebarResetIn()));

//    m_pActionResetOut = new QAction(tr("Reset out point"), this);
//    m_pActionResetOut->setShortcut(tr("p"));
//    connect(m_pActionResetOut, SIGNAL(triggered()), this, SLOT(onSidebarResetOut()));

    connect(m_pDSLRLabView, SIGNAL(signal_error(QString)), this,
            SLOT(onError(QString)));
    connect(m_pDSLRLabView, SIGNAL(signal_stateChanged(ffSequence::ffSequenceState)), this,
            SLOT(onStateChanged(ffSequence::ffSequenceState)));
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
        onStateChanged(ffSequence::justErrored);
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
        onStateChanged(ffSequence::justErrored);
    }
    catch (std::exception e)
    {
        onStateChanged(ffSequence::justErrored);
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

    onStateChanged(m_pDSLRLabView->getState());
}

void MainWindow::onStateChanged(ffSequence::ffSequenceState state)
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
        m_pSBEInSpin->setMinimum(ffDefault::NoFrame);
        m_pSBEInSpin->setValue(ffDefault::NoFrame);
        m_pSBEInSpin->setMaximum(ffDefault::NoFrame);
        m_pSBEOutSpin->setMinimum(ffDefault::NoFrame);
        m_pSBEOutSpin->setValue(ffDefault::NoFrame);
        m_pSBEOutSpin->setMaximum(ffDefault::NoFrame);
        m_pSBToolBox->setEnabled(false);
        break;
    case (ffSequence::isLoading):
    case (ffSequence::justLoading):
        m_pViewActionGroup->setDisabled(true);
        m_pActionFileOpen->setDisabled(true);
        m_pActionFileExport->setDisabled(true);
        m_pMenuView->setDisabled(true);
        m_pSBToolBox->setDisabled(true);
        m_pSBEInSpin->setMinimum(ffDefault::NoFrame);
        m_pSBEInSpin->setValue(ffDefault::NoFrame);
        m_pSBEInSpin->setMaximum(ffDefault::NoFrame);
        m_pSBEOutSpin->setMinimum(ffDefault::NoFrame);
        m_pSBEOutSpin->setValue(ffDefault::NoFrame);
        m_pSBEOutSpin->setMaximum(ffDefault::NoFrame);
        m_pSBToolBox->setEnabled(false);
        break;
    case (ffSequence::isValid):
    case (ffSequence::justOpened):
        m_pViewActionGroup->setEnabled(true);
        m_pActionFileOpen->setEnabled(true);
        m_pActionFileExport->setEnabled(true);
        m_pMenuView->setEnabled(true);
        m_pSBToolBox->setEnabled(true);
        break;
    }
}

void MainWindow::onTrimChanged(long in, long out, void */*sender*/)
{
    m_pSBEInSpin->setRange(ffDefault::FirstFrame, out);
    m_pSBEOutSpin->setRange(in, m_pDSLRLabView->getTotalFrames());
    m_pSBEInSpin->setValue(in);
    m_pSBEOutSpin->setValue(out);
}

void MainWindow::onFrameChanged(long frame, void */*sender*/)
{
    m_pSBEInSpin->setMaximum(frame);
    m_pSBEOutSpin->setMinimum(frame);
}

void MainWindow::onSidebarViewerPlaneChanged(int plane)
{
    m_pDSLRLabView->setViewerPlane((ffViewer::ViewerPlane)plane);
}

void MainWindow::onSidebarExportPlaneChanged(int plane)
{
    m_pSBEPlaneCombo->setCurrentIndex((ffExportDetails::ExportPlane)plane);
}

void MainWindow::onSidebarSetIn(int in)
{
    // Let the lower level ffSequence test for validity.
    m_pDSLRLabView->getQffSequence()->setExportTrimIn(in, this);
}

void MainWindow::onSidebarSetOut(int out)
{
    // Let the lower level ffSequence object test for validity.
   m_pDSLRLabView->getQffSequence()->setExportTrimOut(out, this);
}

void MainWindow::onSidebarResetIn(void)
{
    m_pDSLRLabView->getQffSequence()->resetExportTrimIn(this);
}

void MainWindow::onSidebarResetOut(void)
{
    m_pDSLRLabView->getQffSequence()->resetExportTrimOut(this);
}
