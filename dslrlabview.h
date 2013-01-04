#ifndef DSLRLABVIEW_H
#define DSLRLABVIEW_H

#include <QtGui>
#include <exception>

#include "textpill.h"
#include "ffsequence.h"
#include "qbasegraphicsview.h"

#define DSLRVIEW_SCALE_INCREMENT                10
// Default speed of scaling for zooming in and out.
#define DSLRVIEW_WHEEL_SCALEFACTOR              1.15f
#define DSLRVIEW_OVERLAY_MARGIN                 80
// Animations and Transitions
#define DSLRVIEW_DURATION_INTROFADEIN           250

#define INITANIM                                10

#define PROGRESS_FADE_DURATION                  1000
#define PROGRESS_HEIGHT                         6
#define PROGRESS_MAXIMUM                        1000

#define DSLRVIEW_TRANSPARENT                    0.0
#define DSLRVIEW_OPAQUE                         1.0

#define TIMELINE_DURATION                       350
#define TIMELINE_ZOOM_UPDATE                    16
#define TIMELINE_PROGRESS_UPDATE                16
#define TIMELINE_PROGRESS_DELAY                 0.05

#define DEFAULT_PADDING                         10
#define TEXT_PADDING_X                          DEFAULT_PADDING
#define TEXT_PADDING_Y                          DEFAULT_PADDING

#define MAXIMUM_SCALE                           24.00
#define MINIMUM_SCALE                           0.250

class QffSequence : public QObject, public ffSequence
{
    Q_OBJECT

signals:
    void signal_progressStart(void);
    void signal_progress(double);
    void signal_progressEnd(void);
    void signal_justLoading(void);
    void signal_justOpened(void);
    void signal_justClosed(void);
    void signal_justErrored(void);

private:
    void onProgressStart(void);
    void onProgress(double);
    void onProgressEnd(void);
    void onJustLoading(void);
    void onJustOpened(void);
    void onJustClosed(void);
    void onJustErrored(void);

public:
    explicit QffSequence(QWidget *parent = 0) :
        QObject(parent), ffSequence() {}
};

class DSLRLabView : public QWidget
{
    Q_OBJECT

public:
    explicit DSLRLabView(QWidget *parent = 0);
    ~DSLRLabView();

    void resetTransform(void);
    void updateCurrentFrame(long);
    void fitToView(void);

    long getTotalFrames(void);
    QString getFileURI(void);

    ffRawFrame::PlaneType getDisplayPlane(void);
    void setDisplayPlane(ffRawFrame::PlaneType);

    ffSequence::ffSequenceState getState(void);
    void openSequence(char *);
    void saveSequence(char *, long, long);
    void closeSequence(void);
    QffSequence * getQffSequence(void);

    QGraphicsPixmapItem* getGraphicsPixmapItem(void);
    QTextPill* getTextPillItem(void);

signals:
    void signal_frameChanged(long);
    void signal_error(QString);
    void signal_updateUI(ffSequence::ffSequenceState);

public slots:
    void onScaleTimeslice(qreal x);
    void onScaleAnimFinished(void);
    void onError(QString);
    void onProgressStart(void);
    void onProgress(double);
    void onProgressEnd(void);
    void onProgressAnimation(qreal);
    void onJustLoading(void);
    void onJustOpened(void);
    void onJustClosed(void);
    void onJustErrored(void);
    void onUpdateUI(ffSequence::ffSequenceState);
    void onFrameChange(int);

private:
    QGraphicsAnchorLayout                  *m_pGraphicsAnchorLayout;
    QBaseGraphicsView                      *m_pGraphicsView;
    QBaseGraphicsView                      *m_pGraphicsViewOverlay;
    QGraphicsWidget                        *m_pOverlayAnchor;

    QGraphicsScene                         *m_pGraphicsScene;
    QGraphicsScene                         *m_pGraphicsSceneOverlay;

    QGraphicsPixmapItem                    *m_pGraphicsPixmapItem;

    QffSequence                     *m_pffSequence;

    QTimeLine                              *m_pTimeLine;

    QGraphicsOpacityEffect                 *m_pFadePixmap;
    QPropertyAnimation                     *m_pFadePixmapAnimation;

    QGraphicsOpacityEffect                 *m_pFadeProgressBar;
    QPropertyAnimation                     *m_pFadeProgressBarAnimation;

    QGraphicsOpacityEffect                 *m_pFadeFrameScrubber;
    QPropertyAnimation                     *m_pFadeFrameScrubberAnimation;

    QTimeLine                              *m_pProgressTimeline;

    QProgressBar                           *m_pProgressBar;
    QGraphicsWidget                        *m_pgwProgressBar;
    QTextPill                              *m_pTextPill;
    QSlider                                *m_pSlider;
    QGraphicsWidget                        *m_pgwSlider;

    ffRawFrame::PlaneType                   m_displayPlane;

    int                                     _numScheduledScalings;
    int                                     _targetProgress;

    void createObjects(void);
    void createAnimations(void);

    void initObjects(void);

    void wheelEvent(QWheelEvent *);
    void resizeEvent(QResizeEvent *);

    void mouseReleaseEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
};

#endif // DSLRLABVIEW_H
