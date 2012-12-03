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
    m_pffSequence(NULL)
{
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    //Set-up the scene

    //Scene->addPixmap(QPixmap("/home/aphorism/Pictures/Screenshot from 2012-11-19 10:05:44.png"));

    //Populate the scene
    //for(int x = 0; x < 1000; x = x + 25) {
    //    for(int y = 0; y < 1000; y = y + 25) {

    //        if(x % 100 == 0 && y % 100 == 0) {
    //            Scene->addRect(x, y, 2, 2);

    //            QString pointString;
    //            QTextStream stream(&pointString);
    //            stream << "(" << x << "," << y << ")";
    //            QGraphicsTextItem* item = Scene->addText(pointString);
    //            item->setPos(x, y);
    //       } else {
    //            Scene->addRect(x, y, 1, 1);
    //        }
    //    }
    //}

    //Set-up the view
    //
    //setCenter(QPointF(2560.0/2.0, 1440.0/2)); //A modified version of centerOn(), handles special cases
    //setCursor(Qt::OpenHandCursor);

    m_pGraphicsScene = new QGraphicsScene(this);
    setScene(m_pGraphicsScene);
    //setSceneRect(0, 0, 2560, 1440);

    setBackgroundRole(QPalette::Dark);

    m_pGraphicsPixmapItem = new QGraphicsPixmapItem;
    m_pGraphicsScene->addItem(m_pGraphicsPixmapItem);
    m_pffSequence = new ffSequence;
}

DSLRLabView::~DSLRLabView()
{
    delete m_pffSequence;
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
void DSLRLabView::setCenter(const QPointF& centerPoint)
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

    if(bounds.contains(centerPoint))
        //We are within the bounds
        CurrentCenterPoint = centerPoint;
    else
    {
        //We need to clamp or use the center of the screen
        if(visibleArea.contains(sceneBounds))
            //Use the center of scene ie. we can see the whole scene
            CurrentCenterPoint = sceneBounds.center();
        else
        {
            CurrentCenterPoint = centerPoint;

            //We need to clamp the center. The centerPoint is too large
            if(centerPoint.x() > bounds.x() + bounds.width())
                CurrentCenterPoint.setX(bounds.x() + bounds.width());
            else if(centerPoint.x() < bounds.x())
                CurrentCenterPoint.setX(bounds.x());

            if(centerPoint.y() > bounds.y() + bounds.height())
                CurrentCenterPoint.setY(bounds.y() + bounds.height());
            else if(centerPoint.y() < bounds.y())
                CurrentCenterPoint.setY(bounds.y());
        }
    }

    //Update the scrollbars
    centerOn(CurrentCenterPoint);
}

void DSLRLabView::setCurrentFrame(long frame)
{
    ffRawFrame *pRawFrame = m_pffSequence->getRawFrame(frame);

    QImage *pImage = new QImage(pRawFrame->m_pY,
                          m_pffSequence->getLumaSize().m_width,
                          m_pffSequence->getLumaSize().m_height,
                          QImage::Format_Indexed8);

    //QPixmap *pPixmap = new QPixmap(QPixmap::fromImage(*pImage));
    m_pGraphicsPixmapItem->setPixmap(QPixmap::fromImage(*pImage));

    //delete pPixmap;
    delete pImage;
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
        setCurrentFrame(m_pffSequence->getCurrentFrame());
        setSceneRect(0, 0,
                     m_pffSequence->getLumaSize().m_width,
                     m_pffSequence->getLumaSize().m_height);
        //setCenter(QPointF(0, 0));
        fitToView();
    }
    catch (ffError eff)
    {
        throw;
    }

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
        LastPanPoint = event->pos();
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
        LastPanPoint = QPoint();
    }
}

/**
*Handles the mouse move event
*/
void DSLRLabView::mouseMoveEvent(QMouseEvent* event)
{
    if(!LastPanPoint.isNull())
    {
        //Get how much we panned
        QPointF delta = mapToScene(LastPanPoint) - mapToScene(event->pos());
        LastPanPoint = event->pos();

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

