#ifndef QGRAPHICSTRIMSLIDER_H
#define QGRAPHICSTRIMSLIDER_H

#include <QtGui>
#include "ffsequence.h"

class QGraphicsTrimSlider : public QGraphicsWidget
{
    Q_OBJECT
signals:
    void signal_trimChanged(ffTrim);
    void signal_valueChanged(long);

private:
    QColor                  m_colorInOut;
    QColor                  m_colorBackground;
    QColor                  m_colorPen;
    QRectF                  m_rectF;
    QPainterPath            m_painterPath;
    QPainterPath            m_painterPathTrim;

    long                    m_value;
    long                    m_minValue;
    long                    m_maxValue;
    long                    m_trimIn;
    long                    m_trimOut;
    float                   m_position;

    void updatePaths(void);

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    void createObjects(void);
    void initObjects(void);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);

    virtual void resizeEvent(QGraphicsSceneResizeEvent *);

    QPainterPath shape() const;

public:
    enum Default
    {
        Width = 100,
        Height = 16,
        MinimumValue = 0,
        MaximumValue = 100,
        ValueSliderWidth = 6,
        ValueSliderPen = 2,
        ColorDarkGray = 64,
        ColorMediumGray = 100,
        ColorLightGray = 140
    };

    QGraphicsTrimSlider(QGraphicsItem *parent = 0);
    ~QGraphicsTrimSlider();

    void setTrim(long, long);
    void setTrim(ffTrim);
    void setTrimIn(long);
    void setTrimOut(long);

    ffTrim getTrim(void);

    void setMaximum(long);
    void setMinimum(long);
    void setValue(long);

    long maximum(void);
    long minimum(void);
    long value(void);
};

#endif // QGRAPHICSTRIMSLIDER_H
