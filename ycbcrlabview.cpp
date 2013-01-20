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

#include "ycbcrlabview.h"

// *******
// * QffSequence
// *******

QffSequence::QffSequence(QWidget *parent) :
    QObject(parent), ffSequence()
{
    qRegisterMetaType<ffSequenceState>("ffSequenceState");
}

void QffSequence::onProgressStart(void)
{
    emit signal_progressStart();
}

void QffSequence::onProgress(double factor)
{
    emit signal_progress(factor);
}

void QffSequence::onProgressEnd(void)
{
    emit signal_progressEnd();
}

void QffSequence::onJustLoading(void)
{
    emit signal_justLoading();
}

void QffSequence::onJustOpened(void)
{
    emit signal_justOpened();
}

void QffSequence::onJustClosed(void)
{
    emit signal_justClosed();
}

//void QffSequence::onJustErrored(void)
//{
//    emit signal_justErrored();
//}

void QffSequence::onExportTrimChanged(long in, long out, void *sender)
{
    emit signal_exportTrimChanged(in, out, sender);
}

void QffSequence::onExportPlaneChanged(ffExportDetails::ExportPlane plane,
                                       void *sender)
{
    emit signal_exportPlaneChanged(plane, sender);
}

void QffSequence::onFrameChanged(long frame, void *sender)
{
    emit signal_frameChanged(frame, sender);
}

// *******
// * QYCBCRLabView
// *******
YCbCrLabView::YCbCrLabView(QWidget *parent) :
    QWidget(parent),
    m_pGraphicsView(NULL),
    m_pGraphicsViewOverlay(NULL),
    m_pGraphicsScene(NULL),
    m_pGraphicsSceneOverlay(NULL)
{
    createObjects();
    createActions();
    initObjects();
    createAnimations();
}

void YCbCrLabView::createObjects(void)
{
    m_pGraphicsView = new QBaseGraphicsView(this);
    m_pGraphicsViewOverlay = new QBaseGraphicsView(this, m_pGraphicsView);

    m_pGraphicsScene = new QGraphicsScene(this);
    m_pGraphicsSceneOverlay = new QGraphicsScene(this);

    //m_pOverlayAnchor = new QGraphicsWidget;
    //m_pGraphicsAnchorLayout = new QGraphicsAnchorLayout;

    m_pSlider = new QGraphicsTrimSlider;

    m_pProgressBar = new QProgressBar;
    m_pProgressTimeline = new QTimeLine(TIMELINE_DURATION, this);
    m_pTextPill = new QTextPill;

    m_pTimeLine = new QTimeLine(TIMELINE_DURATION, this);

    m_pGraphicsPixmapItem = new QGraphicsPixmapItem;

    m_pffSequence = new QffSequence;
}

void YCbCrLabView::createActions()
{
    m_pShortcutTrimIn = new QShortcut(tr("i"), this);
    m_pShortcutTrimOut = new QShortcut(tr("o"), this);
}

void YCbCrLabView::initObjects(void)
{
    m_pGraphicsView->setScene(m_pGraphicsScene);
    m_pGraphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    m_pGraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    m_pGraphicsViewOverlay->setStyleSheet("background:transparent;");
    m_pGraphicsViewOverlay->setFrameShape(QFrame::NoFrame);
    m_pGraphicsViewOverlay->setScene(m_pGraphicsSceneOverlay);

    m_pGraphicsSceneOverlay->addItem(m_pTextPill);
    m_pGraphicsSceneOverlay->addItem(m_pSlider);
    m_pSlider->setOpacity(YCBCRVIEW_TRANSPARENT);

    m_pTextPill->setPos(TEXT_PADDING_X, TEXT_PADDING_Y);

    m_pgwProgressBar = m_pGraphicsSceneOverlay->addWidget(m_pProgressBar);
    m_pProgressBar->setMaximumHeight(PROGRESS_HEIGHT);
    m_pProgressBar->setTextVisible(false);
    m_pgwProgressBar->setOpacity(YCBCRVIEW_TRANSPARENT);

    m_pGraphicsViewOverlay->addActive(m_pSlider);

    m_pGraphicsScene->addItem(m_pGraphicsPixmapItem);

    m_pProgressTimeline->setUpdateInterval(TIMELINE_PROGRESS_UPDATE);
    m_pTimeLine->setUpdateInterval(TIMELINE_ZOOM_UPDATE);

    m_viewerPlane = ffViewer::Y;

    connect(m_pTimeLine, SIGNAL(valueChanged(qreal)),
            SLOT(onScaleTimeslice(qreal)));
    connect(m_pTimeLine, SIGNAL(finished()),
            SLOT(onScaleAnimFinished()));
//    connect(this, SIGNAL(signal_error(QString)), this,
//            SLOT(onError(QString)));
    connect(m_pffSequence, SIGNAL(signal_progressStart()), this,
            SLOT(onProgressStart()));
    connect(m_pffSequence, SIGNAL(signal_progress(double)), this,
            SLOT(onProgress(double)));
    connect(m_pffSequence, SIGNAL(signal_progressEnd()), this,
            SLOT(onProgressEnd()));
    connect(m_pffSequence, SIGNAL(signal_justLoading()), this,
            SLOT(onJustLoading()));
    connect(m_pffSequence, SIGNAL(signal_justOpened()), this,
            SLOT(onJustOpened()));
    connect(m_pffSequence, SIGNAL(signal_justClosed()), this,
            SLOT(onJustClosed()));
//    connect(m_pffSequence, SIGNAL(signal_justErrored()), this,
//            SLOT(onJustErrored()));
    connect(m_pProgressTimeline, SIGNAL(valueChanged(qreal)), this,
            SLOT(onProgressAnimation(qreal)));
    connect(m_pSlider, SIGNAL(signal_valueChanged(long)), this,
            SLOT(onSliderChanged(long)));
    connect(m_pffSequence, SIGNAL(signal_frameChanged(long,void*)), this,
            SLOT(onFrameChanged(long,void*)));
    connect(m_pffSequence,
            SIGNAL(signal_exportTrimChanged(long,long,void*)),
            this, SLOT(onExportTrimChanged(long,long,void*)));
    connect(m_pShortcutTrimIn, SIGNAL(activated()), this,
            SLOT(onExportTrimInPressed()));
    connect(m_pShortcutTrimOut, SIGNAL(activated()), this,
            SLOT(onExportTrimOutPressed()));
    connect(m_pffSequence, SIGNAL(
                signal_exportPlaneChanged(
                    ffExportDetails::ExportPlane,void*)), this,
            SLOT(onExportPlaneChanged(
                     ffExportDetails::ExportPlane,void*)));
    connect(this, SIGNAL(signal_stateChanged(
                             ffSequenceState)),
            this, SLOT(onStateChanged(ffSequenceState)));

    m_pGraphicsViewOverlay->viewport()->installEventFilter(
                m_pGraphicsViewOverlay);
}

void YCbCrLabView::createAnimations(void)
{
    m_pFadePixmap = new QGraphicsOpacityEffect;
    m_pFadePixmapAnimation = new QPropertyAnimation(m_pFadePixmap,
                                                    "opacity");
    m_pFadePixmapAnimation->setDuration(YCBCRVIEW_DURATION_INTROFADEIN);
    m_pFadePixmapAnimation->setStartValue(YCBCRVIEW_TRANSPARENT);
    m_pFadePixmapAnimation->setEndValue(YCBCRVIEW_OPAQUE);

    m_pFadeProgressBar = new QGraphicsOpacityEffect;
    m_pFadeProgressBarAnimation =
            new QPropertyAnimation(m_pFadeProgressBar, "opacity");
    m_pFadeProgressBarAnimation->setDuration(PROGRESS_FADE_DURATION);
    m_pFadeProgressBarAnimation->setStartValue(YCBCRVIEW_TRANSPARENT);
    m_pFadeProgressBarAnimation->setEndValue(YCBCRVIEW_OPAQUE);

    m_pFadeFrameScrubber = new QGraphicsOpacityEffect;
    m_pFadeFrameScrubberAnimation =
            new QPropertyAnimation(m_pFadeFrameScrubber, "opacity");
    m_pFadeFrameScrubberAnimation->setDuration(PROGRESS_FADE_DURATION);
    m_pFadeFrameScrubberAnimation->setStartValue(YCBCRVIEW_TRANSPARENT);
    m_pFadeFrameScrubberAnimation->setEndValue(SLIDER_OPACITY);
}

YCbCrLabView::~YCbCrLabView()
{
    // TODO sort out the deletion order to prevent unexpected outs.
    /*delete m_pFadeIn;
    delete m_pTextFadeOut;
    delete m_pFadeInAnimation;
    delete m_pTextFadeOutAnimation;
    delete m_pffSequence;
    delete m_pTextPill;
    delete m_pGraphicsViewOverlay;
    delete m_pGraphicsSceneOverlay;
    delete m_pGraphicsPixmapItem;
    delete m_pGraphicsScene;*/
}


/******************************************************************************
 * Functions
 ******************************************************************************/
void YCbCrLabView::resetTransform()
{
    m_pGraphicsView->resetTransform();
}

void YCbCrLabView::fitToView()
{
    m_pGraphicsView->fitInView(m_pGraphicsPixmapItem, Qt::KeepAspectRatio);
}


long YCbCrLabView::getTotalFrames(void)
{
    return m_pffSequence->getTotalFrames();
}

long YCbCrLabView::getCurrentFrame(void)
{
    return m_pffSequence->getCurrentFrame();
}

void YCbCrLabView::setCurrentFrame(long frame, void *sender)
{
    m_pffSequence->setCurrentFrame(frame, sender);
}

QString YCbCrLabView::getFileURI(void)
{
    return QString::fromStdString(m_pffSequence->getFileURI());
}

ffViewer::ViewerPlane YCbCrLabView::getViewerPlane(void)
{
    return m_viewerPlane;
}

void YCbCrLabView::setViewerPlane(ffViewer::ViewerPlane planeType)
{
    if ((getState() == isValid) && (planeType != m_viewerPlane))
    {
        m_viewerPlane = planeType;
        emit onFrameChanged(m_pffSequence->getCurrentFrame(), this);
        m_pGraphicsView->setSceneRect(m_pGraphicsPixmapItem->boundingRect());
    }
}
ffSequenceState YCbCrLabView::getState(void)
{
    return m_pffSequence->getState();
}

void YCbCrLabView::openSequence(char *fileName)
{
    try
    {
        if (getState() == isValid)
            closeSequence();

        m_pffSequence->readFile(fileName);
        m_pGraphicsView->setSceneRect(0, 0,
                     m_pffSequence->getLumaSize().m_width,
                     m_pffSequence->getLumaSize().m_height);
    }
    catch (ffmpegError eff)
    {
        char errorC[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(eff.getError(), errorC, AV_ERROR_MAX_STRING_SIZE);

        QString message = tr("There was an error attempting to open the "
                             "file. <<FFMPEG: ") + QString(eff.what()) +
                             " (" + QString(errorC) + ")>>";
        emit signal_stateChanged(isInvalid);
        emit signal_error(message);
        throw; // Pass up chain for proper UI handling.
    }
    catch (ffExportError eff)
    {
        QString message = tr("There was an error attempting to open the "
                             "file. <<FFERROR: ") + QString(eff.what()) +
                             " (" + QString::number(eff.getError()) + ")>>";
        emit signal_stateChanged(isInvalid);
        emit signal_error(message);
        throw; // Pass up chain for proper UI handling.
    }
}

void YCbCrLabView::saveSequence(char *fileName)
{
    try
    {
        m_pffSequence->writeFile(fileName);
    }
    catch (ffExportError eff)
    {
        QString message = tr("There was an error attempting to export. "
                             "<<FFEXPORTERROR: ") + QString(eff.what()) +
                             " (" + QString::number(eff.getError()) + ")>>";
        emit signal_error(message);
        throw; // Pass up chain for proper UI handling.
    }
}

void YCbCrLabView::closeSequence(void)
{
    if (getState() == isValid)
        m_pffSequence->closeFile();
}

QffSequence * YCbCrLabView::getQffSequence(void)
{
    return m_pffSequence;
}

QGraphicsPixmapItem* YCbCrLabView::getGraphicsPixmapItem(void)
{
    return m_pGraphicsPixmapItem;
}

QTextPill* YCbCrLabView::getTextPillItem(void)
{
    return m_pTextPill;
}

/******************************************************************************
 * Slots
 ******************************************************************************/
void YCbCrLabView::onSliderChanged(long frame)
{
    setCurrentFrame(frame, m_pSlider);
}

void YCbCrLabView::onScaleTimeslice(qreal)
{
    QTransform matrix(m_pGraphicsView->matrix());
    qreal factor = 1.0 + qreal(m_numScheduledScalings) / 500.0;
    qreal scalefactor = matrix.m11() * factor;

    if ((scalefactor >= MAXIMUM_SCALE) && (factor > 1.0))
        scalefactor = MAXIMUM_SCALE;
    else if ((scalefactor <= MINIMUM_SCALE) && (factor < 1.0))
        scalefactor = MINIMUM_SCALE;

    matrix.setMatrix(scalefactor, matrix.m12(), matrix.m13(),
                     matrix.m21(), scalefactor, matrix.m23(),
                     matrix.m31(), matrix.m32(), matrix.m33());

    m_pGraphicsView->setTransform(QTransform(matrix));
}

void YCbCrLabView::onScaleAnimFinished(void)
{
    if (m_numScheduledScalings > 0)
        m_numScheduledScalings--;
    else
        m_numScheduledScalings++;
}

//void YCBCRLabView::onError(QString)
//{
////    emit signal_stateChanged(ffSequence::justErrored);
//}

void YCbCrLabView::onProgressStart(void)
{
    m_pProgressBar->setMinimum(0);
    m_pProgressBar->setMaximum(PROGRESS_MAXIMUM);
    m_pProgressBar->setValue(0);

    m_pgwProgressBar->setGraphicsEffect(m_pFadeProgressBar);
    m_pFadeProgressBarAnimation->setDirection(QAbstractAnimation::Forward);
    m_pgwProgressBar->setOpacity(YCBCRVIEW_OPAQUE);
    m_pFadeProgressBarAnimation->start();
}

void YCbCrLabView::onProgress(double factor)
{
    m_targetProgress = (factor * PROGRESS_MAXIMUM) +
            PROGRESS_MAXIMUM * TIMELINE_PROGRESS_DELAY;
    if (m_pProgressTimeline->state() != QTimeLine::Running)
        m_pProgressTimeline->start();
}

void YCbCrLabView::onProgressEnd(void)
{
    m_pgwProgressBar->setGraphicsEffect(m_pFadeProgressBar);
    m_pFadeProgressBarAnimation->setDirection(QAbstractAnimation::Backward);
    m_pFadeProgressBarAnimation->start();
}

void YCbCrLabView::onProgressAnimation(qreal)
{
    if (m_targetProgress >= m_pProgressBar->value())
    {
        int step = (((double)m_targetProgress -
                     (double)m_pProgressBar->value()) *
                    ((double)TIMELINE_PROGRESS_UPDATE /
                     (double)TIMELINE_DURATION)) + 1.5;
                ;
        m_pProgressBar->setValue(m_pProgressBar->value() + step);
        m_pProgressBar->update();
    }
}

void YCbCrLabView::onJustLoading(void)
{
    emit signal_stateChanged(justLoading);
}

void YCbCrLabView::onJustOpened(void)
{
    emit signal_stateChanged(justOpened);
}

void YCbCrLabView::onJustClosed(void)
{
    emit signal_stateChanged(justClosed);
}

//void YCBCRLabView::onJustErrored(void)
//{
//    emit signal_stateChanged(ffSequence::justErrored);
//}

void YCbCrLabView::onStateChanged(ffSequenceState state)
{
    switch (state)
    {
    case (isValid):
        break;
    case (isInvalid):
        disconnect(m_pSlider, SIGNAL(signal_valueChanged(long)), this,
                   SLOT(onSliderChanged(long)));
        m_pSlider->setEnabled(false);
        m_pSlider->setMinimum(0);
        m_pSlider->setValue(0);
        m_pSlider->setMaximum(0);
        break;
    case (justLoading):
    case (isLoading):
        disconnect(m_pSlider, SIGNAL(signal_valueChanged(long)), this,
                   SLOT(onSliderChanged(long)));
        m_pSlider->setEnabled(false);
        m_pSlider->setMinimum(0);
        m_pSlider->setValue(0);
        m_pSlider->setMaximum(0);
        break;
    case (justOpened):
        fitToView();
        m_pSlider->setMinimum(ffDefault::FirstFrame);
        m_pSlider->setMaximum(getTotalFrames());
        m_pSlider->setValue(ffDefault::FirstFrame);
        connect(m_pSlider, SIGNAL(signal_valueChanged(long)), this,
                SLOT(onSliderChanged(long)));
        m_pSlider->setEnabled(true);

        m_pSlider->setGraphicsEffect(m_pFadeFrameScrubber);
        m_pFadeFrameScrubberAnimation->setDirection(
                    QAbstractAnimation::Forward);
        m_pSlider->setOpacity(YCBCRVIEW_OPAQUE);
        m_pFadeFrameScrubberAnimation->start();

        // FUTURE_RACE
        m_pGraphicsPixmapItem->setGraphicsEffect(m_pFadePixmap);
        m_pFadePixmapAnimation->setDirection(QAbstractAnimation::Forward);
        m_pFadePixmapAnimation->start();

        m_pTextPill->start(tr("Loaded file ") +
                           QString::fromStdString(m_pffSequence->getFileURI()));
        break;
    case (justClosed):
        m_pGraphicsPixmapItem->setGraphicsEffect(m_pFadePixmap);
        m_pFadePixmapAnimation->setDirection(QAbstractAnimation::Backward);
        m_pFadePixmapAnimation->start();

        m_pSlider->setGraphicsEffect(m_pFadeFrameScrubber);
        m_pFadeFrameScrubberAnimation->setDirection(QAbstractAnimation::Backward);
        m_pFadeFrameScrubberAnimation->start();
        break;
    }
}

void YCbCrLabView::onFrameChanged(long frame, void */*sender*/)
{
    QImage *pImage = NULL;
    ffRawFrame *pRawFrame = m_pffSequence->getRawFrame(frame);

    switch (m_viewerPlane)
    {
    case (ffViewer::RGB):
    case (ffViewer::Y):
        pImage = new QImage(pRawFrame->m_pY,
                            m_pffSequence->getLumaSize().m_width,
                            m_pffSequence->getLumaSize().m_height,
                            QImage::Format_Indexed8);
        break;
    case (ffViewer::Cb):
        pImage = new QImage(pRawFrame->m_pCb,
                            m_pffSequence->getChromaSize().m_width,
                            m_pffSequence->getChromaSize().m_height,
                            QImage::Format_Indexed8);
        break;
    case (ffViewer::Cr):
        pImage = new QImage(pRawFrame->m_pCr,
                            m_pffSequence->getChromaSize().m_width,
                            m_pffSequence->getChromaSize().m_height,
                            QImage::Format_Indexed8);
        break;
    }
    m_pGraphicsPixmapItem->setPixmap(QPixmap::fromImage(*pImage));
    delete pImage;
    update();
    m_pTextPill->start(tr("Frame ") +
                       QString::number(m_pffSequence->getCurrentFrame()));
}

void YCbCrLabView::onExportTrimChanged(long in, long out, void *)
{
    if ((in != m_pSlider->getTrim().m_in) &&
            (out != m_pSlider->getTrim().m_out))
    {
        if (in != m_pSlider->getTrim().m_in)
            m_pTextPill->start(tr("Export trim in and out updated to ") +
                               QString::number(in) + ":" +
                               QString::number(out));
    }
        else if (in != m_pSlider->getTrim().m_in)
    {
            m_pTextPill->start(tr("Export trim in updated to ") +
                               QString::number(in));
    }
        else
    {
            m_pTextPill->start(tr("Export trim out updated to ") +
                               QString::number(out));
    }
    m_pSlider->setTrim(in, out);
}

void YCbCrLabView::onExportTrimInPressed()
{
    getQffSequence()->setExportTrimIn(m_pSlider->value(), m_pShortcutTrimIn);
}

void YCbCrLabView::onExportTrimOutPressed()
{
    getQffSequence()->setExportTrimOut(m_pSlider->value(), m_pShortcutTrimOut);
}

void YCbCrLabView::onExportPlaneChanged(ffExportDetails::ExportPlane, void *)
{
    // Pass. TODO Update TextPill
}

/*****************************************************************************
 * Event Overrides
 *****************************************************************************/
void YCbCrLabView::wheelEvent(QWheelEvent* event)
{
    if (event->orientation() == Qt::Vertical)
    {
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 10;

        m_numScheduledScalings += numSteps;
        if (m_numScheduledScalings * numSteps < 0)
            m_numScheduledScalings = numSteps;

        if (m_pTimeLine->state() != QTimeLine::Running)
            m_pTimeLine->start();
    }
}

void YCbCrLabView::resizeEvent(QResizeEvent *event)
{
    QRect rect(QPoint(0,0), event->size());
    m_pGraphicsView->setGeometry(rect);
    m_pGraphicsViewOverlay->setGeometry(rect);
    m_pGraphicsViewOverlay->setSceneRect(rect);

    QSizeF dest = event->size() / 2;

    m_pgwProgressBar->setGeometry(dest.width() / 2, dest.height() -
                                  (PROGRESS_HEIGHT / 2),
                                  dest.width(), PROGRESS_HEIGHT);

    m_pSlider->setGeometry(SliderPadding, rect.size().height() -
                           m_pSlider->geometry().height() - SliderPadding,
                           rect.width() - (2 * SliderPadding),
                           m_pSlider->geometry().height());
    m_pSlider->update();
    m_pgwProgressBar->update();
}

void YCbCrLabView::mouseReleaseEvent(QMouseEvent *event)
{
    m_pGraphicsView->eventFilter(m_pGraphicsView, event);
    event->ignore();
}

void YCbCrLabView::mousePressEvent(QMouseEvent *event)
{
    m_pGraphicsView->eventFilter(m_pGraphicsView, event);
    event->ignore();
}

void YCbCrLabView::mouseMoveEvent(QMouseEvent *event)
{
    m_pGraphicsView->eventFilter(m_pGraphicsView, event);
    event->ignore();
}
