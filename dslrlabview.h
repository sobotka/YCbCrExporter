#ifndef DSLRLABVIEW_H
#define DSLRLABVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include "textpill.h"
#include "ffsequence.h"

#define DSLRVIEW_SCALE_INCREMENT                10
// Default speed of scaling for zooming in and out.
#define DSLRVIEW_WHEEL_SCALEFACTOR              1.15f
#define DSLRVIEW_OVERLAY_MARGIN                 80

class DSLRLabView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit DSLRLabView(QWidget *parent = 0);
    ~DSLRLabView();

    void fadeUp();
    void setScale(float, QPoint);
    void updateCurrentFrame(long);
    void fitToView(void);
    void openFile(char*);
    long getTotalFrames(void);
    bool isValidSequence(void);
    void sequenceNew(void);
    QGraphicsPixmapItem* getGraphicsPixmapItem(void) { return m_pGraphicsPixmapItem; }
    QPointF getCenter() { return m_currentCentrePointF; }

signals:
    void signal_sequenceNew(void);
    void signal_frameChanged(long);

public slots:

private:
    QGraphicsScene                         *m_pGraphicsScene;
    QGraphicsView                          *m_pGraphicsViewOverlay;
    QGraphicsScene                         *m_pGraphicsSceneOverlay;
    QGraphicsPixmapItem                    *m_pGraphicsPixmapItem;
    ffSequence                             *m_pffSequence;
    TextPill                               *m_pTextPill;
    QGraphicsOpacityEffect                 *m_pFadeUp;
    QGraphicsOpacityEffect                 *m_pTextFade;
    QPropertyAnimation                     *m_pFadeUpAnimation;
    QPropertyAnimation                     *m_pTextAnimation;

    QPointF m_currentCentrePointF;
    QPoint m_lastPanPointF;

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
