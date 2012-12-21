#include "dslrlabview.h"

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextStream>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

#include <exception>

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

    m_pTimeLine = new QTimeLine(DSLRVIEW_ZOOM_DURATION, this);

    m_pTextPill = new QTextPill;

    m_pGraphicsPixmapItem = new QGraphicsPixmapItem;

    m_pffSequence = new ffSequence;
}

void DSLRLabView::initObjects(void)
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);

    m_pGraphicsView->setBackgroundRole(QPalette::Dark);
    m_pGraphicsView->setFrameShape(QFrame::Box);
    m_pGraphicsView->setScene(m_pGraphicsScene);
    m_pGraphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    m_pGraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    m_pGraphicsViewOverlay->setStyleSheet("background:transparent;");
    m_pGraphicsViewOverlay->setFrameShape(QFrame::NoFrame);
    m_pGraphicsViewOverlay->setScene(m_pGraphicsSceneOverlay);
    m_pGraphicsViewOverlay->setAttribute(Qt::WA_TransparentForMouseEvents);

    m_pGraphicsScene->addItem(m_pGraphicsPixmapItem);

    m_pGraphicsSceneOverlay->addItem(m_pTextPill);

    m_pTimeLine->setUpdateInterval(DSLRVIEW_ZOOM_UPDATE);

    m_pTextPill->setPos(TEXT_PADDING_X, TEXT_PADDING_Y);

    connect(m_pTimeLine, SIGNAL(valueChanged(qreal)), SLOT(scalingTime(qreal)));
    connect(m_pTimeLine, SIGNAL(finished()), SLOT(animFinished()));
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

void DSLRLabView::scalingTime(qreal)
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

void DSLRLabView::animFinished(void)
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

void DSLRLabView::createAnimations(void)
{
    m_pFadeIn = new QGraphicsOpacityEffect;

    m_pFadeInAnimation = new QPropertyAnimation(m_pFadeIn, "opacity");
    m_pFadeInAnimation->setDuration(DSLRVIEW_DURATION_INTROFADEIN);
    m_pFadeInAnimation->setStartValue(DSLRVIEW_TRANSPARENT);
    m_pFadeInAnimation->setEndValue(DSLRVIEW_OPAQUE);
}

void DSLRLabView::updateCurrentFrame(long frame)
{
    ffRawFrame *pRawFrame = m_pffSequence->getRawFrame(frame);

    QImage *pImage = new QImage(pRawFrame->m_pY,
                                      m_pffSequence->getLumaSize().m_width,
                                      m_pffSequence->getLumaSize().m_height,
                                      QImage::Format_Indexed8);

    m_pGraphicsPixmapItem->setPixmap(QPixmap::fromImage(*pImage));
    delete pImage;
}

void DSLRLabView::fitToView()
{
    m_pGraphicsView->fitInView(m_pGraphicsPixmapItem, Qt::KeepAspectRatio);
}

void DSLRLabView::openFile(char *fileName)
{
    try
    {
        m_pffSequence->openFile(fileName);
        m_pGraphicsView->setSceneRect(0, 0,
                     m_pffSequence->getLumaSize().m_width,
                     m_pffSequence->getLumaSize().m_height);
        emit signal_sequenceNew();
    }
    catch (ffError eff)
    {
        throw;
    }

}

void DSLRLabView::sequenceNew(void)
{
    m_pTextPill->init(QString::fromStdString(m_pffSequence->getFilename()));

    updateCurrentFrame(m_pffSequence->getCurrentFrame());
    fitToView();

    m_pGraphicsPixmapItem->setGraphicsEffect(m_pFadeIn);
    m_pFadeInAnimation->start();
}

long DSLRLabView::getTotalFrames(void)
{
    return m_pffSequence->getTotalFrames();
}

bool DSLRLabView::isValidSequence(void)
{
    return m_pffSequence->isValid();
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
}
