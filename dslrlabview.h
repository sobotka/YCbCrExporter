#ifndef DSLRLABVIEW_H
#define DSLRLABVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QTimeLine>
#include <QApplication>

#include "textpill.h"
#include "ffsequence.h"

#define DSLRVIEW_SCALE_INCREMENT                10
// Default speed of scaling for zooming in and out.
#define DSLRVIEW_WHEEL_SCALEFACTOR              1.15f
#define DSLRVIEW_OVERLAY_MARGIN                 80
// Animations and Transitions
#define DSLRVIEW_DURATION_INTROFADEIN           500
#define DSLRVIEW_DURATION_TEXTFADEOUT           3000
#define DSLRVIEW_TRANSPARENT                    0.0
#define DSLRVIEW_OPAQUE                         1.0

#define DSLRVIEW_ZOOM_DURATION                  350
#define DSLRVIEW_ZOOM_UPDATE                    16

#define TEXT_PADDING_X                          10
#define TEXT_PADDING_Y                          10

#define MAXIMUM_SCALE                           10.00
#define MINIMUM_SCALE                           0.250

class QBaseGraphicsView : public QGraphicsView
{
public:
    explicit QBaseGraphicsView(QWidget *parent = 0);
private:
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
};

class DSLRLabView : public QWidget
{
    Q_OBJECT

public:
    QTextPill                              *m_pTextPill; // Public temp.
    explicit DSLRLabView(QWidget *parent = 0);
    ~DSLRLabView();

    void setScale(float, QPoint);
    void resetTransform(void);
    void updateCurrentFrame(long);
    void fitToView(void);
    long getTotalFrames(void);

    bool isValidSequence(void);
    void openSequence(char*);
    void closeSequence(void);

    QGraphicsPixmapItem* getGraphicsPixmapItem(void) { return m_pGraphicsPixmapItem; }

signals:
    void signal_sequenceClose(void);
    void signal_sequenceNew(void);
    void signal_frameChanged(long);
    void signal_error(QString);

public slots:
    void onScaleTimeslice(qreal x);
    void onScaleAnimFinished(void);
    void onSequenceNew(void);
    void onSequenceClose(void);

private:
    QBaseGraphicsView                      *m_pGraphicsView;
    QBaseGraphicsView                      *m_pGraphicsViewOverlay;

    QGraphicsScene                         *m_pGraphicsScene;
    QGraphicsScene                         *m_pGraphicsSceneOverlay;

    QGraphicsPixmapItem                    *m_pGraphicsPixmapItem;

    ffSequence                             *m_pffSequence;

    QTimeLine                              *m_pTimeLine;

    //QTextPill                              *m_pTextPill;

    QGraphicsOpacityEffect                 *m_pFadeIn;
    QPropertyAnimation                     *m_pFadeInAnimation;

    int                                     _numScheduledScalings;

    void createObjects(void);
    void createAnimations(void);

    void initObjects(void);

    virtual void wheelEvent(QWheelEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
};

#endif // DSLRLABVIEW_H
