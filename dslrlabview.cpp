#include "dslrlabview.h"

#include <QtGui>

#include <exception>

// *******
// * progressffSequence
// *******
void progressffSequence::onProgressStart(void)
{
    emit signal_progressStart();
}

void progressffSequence::onProgress(double factor)
{
    emit signal_progress(factor);
}

void progressffSequence::onProgressEnd(void)
{
    emit signal_progressEnd();
}

// *******
// * QBaseGraphicsView
// *******
QBaseGraphicsView::QBaseGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void QBaseGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void QBaseGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void QBaseGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
}

void QBaseGraphicsView::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}

// *******
// * QDSLRLabView
// *******
DSLRLabView::DSLRLabView(QWidget *parent) :
    QWidget(parent),
    m_pGraphicsView(NULL),
    m_pGraphicsViewOverlay(NULL),
    m_pGraphicsScene(NULL),
    m_pGraphicsSceneOverlay(NULL)
{
    createObjects();
    initObjects();
    createAnimations();
}

void DSLRLabView::createObjects(void)
{
    m_pGraphicsView = new QBaseGraphicsView(this);
    m_pGraphicsViewOverlay = new QBaseGraphicsView(this);

    m_pGraphicsScene = new QGraphicsScene(this);
    m_pGraphicsSceneOverlay = new QGraphicsScene(this);

    m_pProgressBar = new QProgressBar;
    m_pProgressTimeline = new QTimeLine(TIMELINE_DURATION, this);
    m_pTextPill = new QTextPill;

    m_pTimeLine = new QTimeLine(TIMELINE_DURATION, this);

    m_pGraphicsPixmapItem = new QGraphicsPixmapItem;

    m_pffSequence = new progressffSequence;
}

void DSLRLabView::initObjects(void)
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);

    m_pGraphicsView->setBackgroundRole(QPalette::Dark);
    m_pGraphicsView->setScene(m_pGraphicsScene);
    m_pGraphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    m_pGraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    m_pGraphicsViewOverlay->setStyleSheet("background:transparent;");
    m_pGraphicsViewOverlay->setFrameShape(QFrame::NoFrame);
    m_pGraphicsViewOverlay->setScene(m_pGraphicsSceneOverlay);
    m_pGraphicsViewOverlay->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_pGraphicsSceneOverlay->addItem(m_pTextPill);

    m_pTextPill->setPos(TEXT_PADDING_X, TEXT_PADDING_Y);

    m_pProgressBar->setStyleSheet("background:transparent;");
    m_pProgressBar->setMaximumHeight(PROGRESS_HEIGHT);
    m_pProgressBar->setTextVisible(false);
    m_pgwProgressBar = m_pGraphicsSceneOverlay->addWidget(m_pProgressBar);
    m_pgwProgressBar->setOpacity(DSLRVIEW_TRANSPARENT);

    m_pGraphicsScene->addItem(m_pGraphicsPixmapItem);

    m_pProgressTimeline->setUpdateInterval(TIMELINE_PROGRESS_UPDATE);
    m_pTimeLine->setUpdateInterval(TIMELINE_ZOOM_UPDATE);

    m_displayPlane = ffRawFrame::Combined;

    connect(m_pTimeLine, SIGNAL(valueChanged(qreal)),
            SLOT(onScaleTimeslice(qreal)));
    connect(m_pTimeLine, SIGNAL(finished()), SLOT(onScaleAnimFinished()));
    connect(this, SIGNAL(signal_sequenceNew()), this, SLOT(onSequenceNew()));
    connect(this, SIGNAL(signal_sequenceClose()), this,
            SLOT(onSequenceClose()));
    connect(this, SIGNAL(signal_error(QString)), this, SLOT(onError(QString)));
    connect(this, SIGNAL(signal_sequenceStartOpen()), this,
            SLOT(onSequenceStartOpen()));
    connect(m_pffSequence, SIGNAL(signal_progressStart()), this,
            SLOT(onProgressStart()));
    connect(m_pffSequence, SIGNAL(signal_progress(double)), this,
            SLOT(onProgress(double)));
    connect(m_pffSequence, SIGNAL(signal_progressEnd()), this,
            SLOT(onProgressEnd()));
    connect(m_pProgressTimeline, SIGNAL(valueChanged(qreal)), this,
            SLOT(onProgressAnimation(qreal)));
}

void DSLRLabView::createAnimations(void)
{
    m_pFadePixmap = new QGraphicsOpacityEffect;

    m_pFadePixmapAnimation = new QPropertyAnimation(m_pFadePixmap, "opacity");
    m_pFadePixmapAnimation->setDuration(DSLRVIEW_DURATION_INTROFADEIN);
    m_pFadePixmapAnimation->setStartValue(DSLRVIEW_TRANSPARENT);
    m_pFadePixmapAnimation->setEndValue(DSLRVIEW_OPAQUE);

    m_pFadeProgressBar = new QGraphicsOpacityEffect;

    m_pFadeProgressBarAnimation = new QPropertyAnimation(m_pFadeProgressBar,
                                                         "opacity");
    m_pFadeProgressBarAnimation->setDuration(PROGRESS_FADE_DURATION);
    m_pFadeProgressBarAnimation->setStartValue(DSLRVIEW_TRANSPARENT);
    m_pFadeProgressBarAnimation->setEndValue(DSLRVIEW_OPAQUE);
}

DSLRLabView::~DSLRLabView()
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

QGraphicsPixmapItem* DSLRLabView::getGraphicsPixmapItem(void)
{
    return m_pGraphicsPixmapItem;
}

QTextPill* DSLRLabView::getTextPillItem(void)
{
    return m_pTextPill;
}

ffRawFrame::PlaneType DSLRLabView::getDisplayPlane(void)
{
    return m_displayPlane;
}

void DSLRLabView::setDisplayPlane(ffRawFrame::PlaneType planeType)
{
    if ((getState() == ffSequence::isValid) && (planeType != m_displayPlane))
    {
        m_displayPlane = planeType;
        updateCurrentFrame(m_pffSequence->getCurrentFrame());
        m_pGraphicsView->setSceneRect(m_pGraphicsPixmapItem->boundingRect());
    }
}

void DSLRLabView::onScaleTimeslice(qreal)
{
    QTransform matrix(m_pGraphicsView->matrix());
    qreal factor = 1.0 + qreal(_numScheduledScalings) / 500.0;
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

void DSLRLabView::onScaleAnimFinished(void)
{
    if (_numScheduledScalings > 0)
        _numScheduledScalings--;
    else
        _numScheduledScalings++;
}

void DSLRLabView::resetTransform()
{
    m_pGraphicsView->resetTransform();
}

void DSLRLabView::updateCurrentFrame(long frame)
{
    QImage *pImage = NULL;
    ffRawFrame *pRawFrame = m_pffSequence->setCurrentFrame(frame);

    switch (m_displayPlane)
    {
    case (ffRawFrame::Combined):
    case (ffRawFrame::Y):
        pImage = new QImage(pRawFrame->m_pY,
                                    m_pffSequence->getLumaSize().m_width,
                                    m_pffSequence->getLumaSize().m_height,
                                    QImage::Format_Indexed8);
        break;
    case (ffRawFrame::Cb):
        pImage = new QImage(pRawFrame->m_pCb,
                                    m_pffSequence->getChromaSize().m_width,
                                    m_pffSequence->getChromaSize().m_height,
                                    QImage::Format_Indexed8);
        break;
    case (ffRawFrame::Cr):
        pImage = new QImage(pRawFrame->m_pCr,
                                    m_pffSequence->getChromaSize().m_width,
                                    m_pffSequence->getChromaSize().m_height,
                                    QImage::Format_Indexed8);
        break;
    }

    m_pGraphicsPixmapItem->setPixmap(QPixmap::fromImage(*pImage));
    delete pImage;
}

void DSLRLabView::fitToView()
{
    m_pGraphicsView->fitInView(m_pGraphicsPixmapItem, Qt::KeepAspectRatio);
}

void DSLRLabView::onSequenceNew(void)
{
    m_pTextPill->start(tr("Loaded file ") +
                          QString::fromStdString(m_pffSequence->getFilename()));

    updateCurrentFrame(m_pffSequence->getCurrentFrame());
    fitToView();

    m_pgwProgressBar->setGraphicsEffect(m_pFadeProgressBar);
    m_pFadeProgressBarAnimation->setDirection(QAbstractAnimation::Backward);
    m_pFadeProgressBarAnimation->start();

    m_pGraphicsPixmapItem->setGraphicsEffect(m_pFadePixmap);
    m_pFadePixmapAnimation->setDirection(QAbstractAnimation::Forward);
    m_pFadePixmapAnimation->start();
}

void DSLRLabView::onSequenceClose(void)
{
    m_pGraphicsPixmapItem->setGraphicsEffect(m_pFadePixmap);
    m_pFadePixmapAnimation->setDirection(QAbstractAnimation::Backward);
    m_pFadePixmapAnimation->start();
}

void DSLRLabView::onError(QString)
{
    m_pgwProgressBar->setGraphicsEffect(m_pFadeProgressBar);
    m_pFadeProgressBarAnimation->setDirection(QAbstractAnimation::Backward);
    m_pFadeProgressBarAnimation->start();
}

void DSLRLabView::onSequenceStartOpen(void)
{
}

void DSLRLabView::onProgressStart(void)
{
    m_pgwProgressBar->setGraphicsEffect(m_pFadeProgressBar);
    m_pgwProgressBar->setOpacity(DSLRVIEW_OPAQUE);
    m_pFadeProgressBarAnimation->setDirection(QAbstractAnimation::Forward);
    m_pFadeProgressBarAnimation->start();
    m_pProgressBar->setMinimum(0);
    m_pProgressBar->setMaximum(PROGRESS_MAXIMUM);
    m_pProgressBar->setValue(0);
}

void DSLRLabView::onProgress(double factor)
{
    _targetProgress = (factor * PROGRESS_MAXIMUM) +
            PROGRESS_MAXIMUM * TIMELINE_PROGRESS_DELAY;
    if (m_pProgressTimeline->state() != QTimeLine::Running)
        m_pProgressTimeline->start();
}

void DSLRLabView::onProgressEnd(void)
{
    m_pgwProgressBar->setGraphicsEffect(m_pFadeProgressBar);
    m_pFadeProgressBarAnimation->setDirection(QAbstractAnimation::Backward);
    m_pFadeProgressBarAnimation->start();
}

void DSLRLabView::onProgressAnimation(qreal)
{
    if (_targetProgress >= m_pProgressBar->value())
    {
        int step = (((double)_targetProgress -
                     (double)m_pProgressBar->value()) *
                    ((double)TIMELINE_PROGRESS_UPDATE /
                     (double)TIMELINE_DURATION)) + 1.5;
                ;
        m_pProgressBar->setValue(m_pProgressBar->value() + step);
        m_pProgressBar->update();
    }
}

long DSLRLabView::getTotalFrames(void)
{
    return m_pffSequence->getTotalFrames();
}

ffSequence::ffSequenceState DSLRLabView::getState(void)
{
    return m_pffSequence->getState();
}

void DSLRLabView::openSequence(char *fileName)
{
    try
    {
        if (getState() == ffSequence::isValid)
            closeSequence();

        emit signal_sequenceStartOpen();

        m_pffSequence->openFile(fileName);
        m_pGraphicsView->setSceneRect(0, 0,
                     m_pffSequence->getLumaSize().m_width,
                     m_pffSequence->getLumaSize().m_height);
        emit signal_sequenceNew();
    }
    catch (ffmpegError ffeff)
    {
        char errorC[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ffeff.getError(), errorC, AV_ERROR_MAX_STRING_SIZE);

        QString message = tr("FFMPEG: ") + QString(ffeff.what()) + " (" +
                QString(errorC) + ")";
        emit signal_error(message);
    }
    catch (ffError eff)
    {
        QString message = tr("FFSEQUENCE: ") + QString(eff.what()) + " (" +
                QString::number(eff.getError()) + ")";
        emit signal_error(message);
    }
}

void DSLRLabView::closeSequence(void)
{
    if (getState())
    {
        m_pffSequence->closeFile();
        emit signal_sequenceClose();
    }
}

void DSLRLabView::wheelEvent(QWheelEvent* event)
{
    if (event->orientation() == Qt::Vertical)
    {
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 10;

        _numScheduledScalings += numSteps;
        if (_numScheduledScalings * numSteps < 0)
            _numScheduledScalings = numSteps;

        if (m_pTimeLine->state() != QTimeLine::Running)
            m_pTimeLine->start();
    }
}

void DSLRLabView::resizeEvent(QResizeEvent *event)
{
    QRect rect(QPoint(0,0), event->size());
    m_pGraphicsView->setGeometry(rect);
    m_pGraphicsViewOverlay->setGeometry(rect);
    m_pGraphicsViewOverlay->setSceneRect(rect);

    QSizeF dest = event->size() / 2;

    m_pgwProgressBar->setGeometry(dest.width() / 2, dest.height() -
                                  (PROGRESS_HEIGHT / 2),
                                  dest.width(), PROGRESS_HEIGHT);
    m_pgwProgressBar->update();
}
