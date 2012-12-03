#ifndef DSLRLABVIEW_H
#define DSLRLABVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>

#include "ffsequence.h"

#define DSLRVIEW_SCALE_INCREMENT                10
// Default speed of scaling for zooming in and out.
#define DSLRVIEW_WHEEL_SCALEFACTOR              1.15f

class DSLRLabView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit DSLRLabView(QWidget *parent = 0);
    ~DSLRLabView();

    void setScale(float, QPoint);
    void setCurrentFrame(long);
    void fitToView(void);
    void openFile(char*);
    long getTotalFrames(void);
    bool isValidSequence(void);
    QGraphicsPixmapItem* getGraphicsPixmapItem(void) { return m_pGraphicsPixmapItem; }
    QPointF getCenter() { return CurrentCenterPoint; }
signals:
    
public slots:

private:
    QGraphicsScene                         *m_pGraphicsScene;
    QGraphicsPixmapItem                    *m_pGraphicsPixmapItem;
    ffSequence                             *m_pffSequence;

    //Holds the current centerpoint for the view, used for panning and zooming
    QPointF CurrentCenterPoint;

    //From panning the view
    QPoint LastPanPoint;

    //Set the current centerpoint in the
    void setCenter(const QPointF& centerPoint);

    //Take over the interaction
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
};

#endif // DSLRLABVIEW_H
