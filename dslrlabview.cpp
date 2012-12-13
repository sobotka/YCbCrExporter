#include "dslrlabview.h"

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextStream>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

#include <exception>

DSLRLabView::DSLRLabView(QWidget *parent) :
    QGraphicsView(parent), m_pGraphicsScene(NULL), m_pGraphicsPixmapItem(NULL),
    m_pffSequence(NULL), m_pTextPill(NULL), m_pFadeUp(NULL), m_pTextFade(NULL),
    m_pFadeUpAnimation(NULL), m_pTextAnimation(NULL)
{
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setBackgroundRole(QPalette::Dark);

    m_pGraphicsScene = new QGraphicsScene(this);
    setScene(m_pGraphicsScene);

    m_pGraphicsViewOverlay = new QGraphicsView(this);
    m_pGraphicsViewOverlay->setStyleSheet("background:transparent; border:none; margin: 10px;");
    m_pGraphicsSceneOverlay = new QGraphicsScene(this);

    m_pGraphicsViewOverlay->setScene(m_pGraphicsSceneOverlay);

    m_pTextPill = new TextPill;
    m_pGraphicsSceneOverlay->addItem(m_pTextPill);

    m_pTextPill->setText("This is a really, really, really, long, long, long, long, long item.");

    m_pGraphicsPixmapItem = new QGraphicsPixmapItem;

    m_pFadeUp = new QGraphicsOpacityEffect;
    m_pGraphicsPixmapItem->setGraphicsEffect(m_pFadeUp);

    m_pFadeUpAnimation = new QPropertyAnimation(m_pFadeUp, "opacity");
    m_pFadeUpAnimation->setDuration(1000);
    m_pFadeUpAnimation->setStartValue(0);
    m_pFadeUpAnimation->setEndValue(1);

    m_pGraphicsScene->addItem(m_pGraphicsPixmapItem);

    m_pffSequence = new ffSequence;
}

DSLRLabView::~DSLRLabView()
{
    delete m_pffSequence;
    delete m_pTextPill;
    delete m_pGraphicsViewOverlay;
    delete m_pGraphicsSceneOverlay;
    delete m_pGraphicsPixmapItem;
    delete m_pGraphicsScene;
}

/**
 * DSLRLab::setScale
 *      Scales the view's contents.
 **/
void DSLRLabView::setScale(float scaleFactor, QPoint scaleCenterPos)
{
    // Store the position relative to the scene.
    QPointF beforePos(mapToScene(scaleCenterPos));

    // Perform the scale.
    scale(scaleFactor, scaleFactor);

    setCenter((QPointF)getCenter() +
              (beforePos - (QPointF)mapToScene(scaleCenterPos)));
}

/**
  * Sets the current centerpoint.  Also updates the scene's center point.
  * Unlike centerOn, which has no way of getting the floating point center
  * back, SetCenter() stores the center point.  It also handles the special
  * sidebar case.  This function will claim the centerPoint to sceneRec ie.
  * the centerPoint must be within the sceneRec.
  */
//Set the current centerpoint in the
void DSLRLabView::setCenter(const QPointF& centrePoint)
{
    //Get the rectangle of the visible area in scene coords
    QRectF visibleArea = mapToScene(rect()).boundingRect();

    //Get the scene area
    QRectF sceneBounds = sceneRect();

    double boundX = visibleArea.width() / 2.0;
    double boundY = visibleArea.height() / 2.0;
    double boundWidth = sceneBounds.width() - 2.0 * boundX;
    double boundHeight = sceneBounds.height() - 2.0 * boundY;

    //The max boundary that the centerPoint can be to
    QRectF bounds(boundX, boundY, boundWidth, boundHeight);

    if(bounds.contains(centrePoint))
        //We are within the bounds
        m_currentCentrePointF = centrePoint;
    else
    {
        //We need to clamp or use the center of the screen
        if(visibleArea.contains(sceneBounds))
            //Use the center of scene ie. we can see the whole scene
            m_currentCentrePointF = sceneBounds.center();
        else
        {
            m_currentCentrePointF = centrePoint;

            //We need to clamp the center. The centerPoint is too large
            if(centrePoint.x() > bounds.x() + bounds.width())
                m_currentCentrePointF.setX(bounds.x() + bounds.width());
            else if(centrePoint.x() < bounds.x())
                m_currentCentrePointF.setX(bounds.x());

            if(centrePoint.y() > bounds.y() + bounds.height())
                m_currentCentrePointF.setY(bounds.y() + bounds.height());
            else if(centrePoint.y() < bounds.y())
                m_currentCentrePointF.setY(bounds.y());
        }
    }

    //Update the scrollbars
    centerOn(m_currentCentrePointF);
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

void DSLRLabView::fadeUp()
{
    /*if (m_pFadeUp)
    {
        delete m_pFadeUp;
        m_pFadeUp = NULL;
    }
    if (m_pFadeUpAnimation)
    {
        delete m_pFadeUpAnimation;
        m_pFadeUpAnimation = NULL;
    }
    m_pFadeUp = new QGraphicsOpacityEffect;
    m_pFadeUp->setOpacity(0);
    m_pGraphicsPixmapItem->setGraphicsEffect(m_pFadeUp);

    m_pFadeUpAnimation = new QPropertyAnimation(m_pFadeUp, "opacity");
    m_pFadeUpAnimation->setDuration(1000);
    m_pFadeUpAnimation->setStartValue(0);
    m_pFadeUpAnimation->setEndValue(1);
    m_pFadeUpAnimation->start();*/
}

void DSLRLabView::fitToView()
{
    fitInView(m_pGraphicsPixmapItem, Qt::KeepAspectRatio);
}

void DSLRLabView::openFile(char *fileName)
{
    try
    {
        m_pffSequence->openFile(fileName);
        //setCurrentFrame(m_pffSequence->getCurrentFrame());
        setSceneRect(0, 0,
                     m_pffSequence->getLumaSize().m_width,
                     m_pffSequence->getLumaSize().m_height);
        emit signal_sequenceNew();
        //emit signal_frameChanged(m_pffSequence->getCurrentFrame());
        //setCenter(QPointF(0, 0));
        //fitToView();
    }
    catch (ffError eff)
    {
        throw;
    }

}

void DSLRLabView::sequenceNew(void)
{
    updateCurrentFrame(m_pffSequence->getCurrentFrame());
    fitToView();
    m_pGraphicsPixmapItem->show();
    m_pFadeUpAnimation->start();
}

long DSLRLabView::getTotalFrames(void)
{
    return m_pffSequence->getTotalFrames();
}

bool DSLRLabView::isValidSequence(void)
{
    return m_pffSequence->isValid();
}

/**
  * Grab on middle mouse button.
  */
void DSLRLabView::mousePressEvent(QMouseEvent* event)
{
    // Click drag on middle mouse button
    if (event->button() == Qt::MiddleButton)
    {
        m_lastPanPointF = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

/**
  * Release on middle mouse button.
  */
void DSLRLabView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        setCursor(Qt::ArrowCursor);
        m_lastPanPointF = QPoint();
    }
}

/**
*Handles the mouse move event
*/
void DSLRLabView::mouseMoveEvent(QMouseEvent* event)
{
    if(!m_lastPanPointF.isNull())
    {
        //Get how much we panned
        QPointF delta = mapToScene(m_lastPanPointF) - mapToScene(event->pos());
        m_lastPanPointF = event->pos();

        //Update the center ie. do the pan
        setCenter(getCenter() + delta);
    }
}

/**
  * Zoom the view in and out.
  */
void DSLRLabView::wheelEvent(QWheelEvent* event)
{
    if (event->orientation() == Qt::Vertical)
    {
        if(event->delta() > 0)
            //Zoom in
            setScale(DSLRVIEW_WHEEL_SCALEFACTOR, event->pos());
        else
            //Zooming out
            setScale(1.0 / DSLRVIEW_WHEEL_SCALEFACTOR, event->pos());
    }
}

/**
  * Need to update the center so there is no jolt in the
  * interaction after resizing the widget.
  */
void DSLRLabView::resizeEvent(QResizeEvent* event)
{
    //Get the rectangle of the visible area in scene coords
    QRectF visibleArea = mapToScene(rect()).boundingRect();
    setCenter(visibleArea.center());

    //Call the subclass resize so the scrollbars are updated correctly
    QGraphicsView::resizeEvent(event);
}

