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

#ifndef YCBCRLABVIEW_H
#define YCBCRLABVIEW_H

#include <QtGui>
#include <exception>

#include "textpill.h"
#include "ffsequence.h"
#include "qbasegraphicsview.h"
#include "qgraphicstrimslider.h"

#define YCBCRVIEW_SCALE_INCREMENT                10
// Default speed of scaling for zooming in and out.
#define YCBCRVIEW_WHEEL_SCALEFACTOR              1.15f
#define YCBCRVIEW_OVERLAY_MARGIN                 80
// Animations and Transitions
#define YCBCRVIEW_DURATION_INTROFADEIN           250

#define INITANIM                                10

#define PROGRESS_FADE_DURATION                  1000
#define PROGRESS_HEIGHT                         6
#define PROGRESS_MAXIMUM                        1000

#define YCBCRVIEW_TRANSPARENT                    0.0
#define YCBCRVIEW_OPAQUE                         1.0

#define TIMELINE_DURATION                       350
#define TIMELINE_ZOOM_UPDATE                    16
#define TIMELINE_PROGRESS_UPDATE                16
#define TIMELINE_PROGRESS_DELAY                 0.05

#define DEFAULT_PADDING                         10
#define TEXT_PADDING_X                          DEFAULT_PADDING
#define TEXT_PADDING_Y                          DEFAULT_PADDING

#define SLIDER_OPACITY                          0.70

#define MAXIMUM_SCALE                           100.00
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

    // setFunction() Events
    void signal_exportTrimChanged(long, long, void*);
    void signal_exportPlaneChanged(ffExportDetails::ExportPlane, void *);
    void signal_frameChanged(long, void *);

private:
    void onProgressStart(void);
    void onProgress(double);
    void onProgressEnd(void);
    void onJustLoading(void);
    void onJustOpened(void);
    void onJustClosed(void);
//    void onJustErrored(void);
    void onExportTrimChanged(long, long, void *);
    void onExportPlaneChanged(ffExportDetails::ExportPlane, void *);
    void onFrameChanged(long, void *);

public:
    explicit QffSequence(QWidget *parent = 0);
};

class YCbCrLabView : public QWidget
{
    Q_OBJECT

public:
    enum Default
    {
        SliderPadding = 10
    };

    explicit YCbCrLabView(QWidget *parent = 0);
    ~YCbCrLabView();

    void resetTransform(void);
    void fitToView(void);

    long getTotalFrames(void);
    long getCurrentFrame(void);
    void setCurrentFrame(long, void *sender);
    QString getFileURI(void);

    ffViewer::ViewerPlane getViewerPlane(void);
    void setViewerPlane(ffViewer::ViewerPlane);

    ffSequenceState getState(void);
    void openSequence(char *);
    void saveSequence(char *);
    void closeSequence(void);
    QffSequence * getQffSequence(void);

    QGraphicsPixmapItem* getGraphicsPixmapItem(void);
    QTextPill* getTextPillItem(void);

signals:
    void signal_frameChange(long, void *);
    void signal_trimChange(long, long, void *);
    void signal_error(QString);
    void signal_stateChanged(ffSequenceState);

public slots:
    void onSliderChanged(long);
    void onScaleTimeslice(qreal x);
    void onScaleAnimFinished(void);
//    void onError(QString);

    // ffSequence Events
    void onProgressStart(void);
    void onProgress(double);
    void onProgressEnd(void);
    void onProgressAnimation(qreal);
    void onJustLoading(void);
    void onJustOpened(void);
    void onJustClosed(void);
//    void onJustErrored(void);
    void onStateChanged(ffSequenceState);

    // ffSequence setFunction Events
    void onFrameChanged(long, void *);
    void onExportTrimChanged(long in, long out, void *);
    void onExportTrimInPressed(void);
    void onExportTrimOutPressed(void);

    void onExportPlaneChanged(ffExportDetails::ExportPlane, void *);

private:
    QGraphicsAnchorLayout                  *m_pGraphicsAnchorLayout;
    QBaseGraphicsView                      *m_pGraphicsView;
    QBaseGraphicsView                      *m_pGraphicsViewOverlay;
    QGraphicsWidget                        *m_pOverlayAnchor;

    QGraphicsScene                         *m_pGraphicsScene;
    QGraphicsScene                         *m_pGraphicsSceneOverlay;

    QGraphicsPixmapItem                    *m_pGraphicsPixmapItem;

    QffSequence                            *m_pffSequence;

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
    QGraphicsTrimSlider                    *m_pSlider;

    ffViewer::ViewerPlane                  m_viewerPlane;

    QShortcut                              *m_pShortcutTrimIn;
    QShortcut                              *m_pShortcutTrimOut;

    int                                     m_numScheduledScalings;
    int                                     m_targetProgress;

    void createObjects(void);
    void createActions(void);
    void createAnimations(void);

    void initObjects(void);

    void wheelEvent(QWheelEvent *);
    void resizeEvent(QResizeEvent *);

    void mouseReleaseEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
};

#endif // YCBCRLABVIEW_H
