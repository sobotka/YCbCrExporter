#include "qgraphicstrimslider.h"

void QGraphicsTrimSlider::paint(QPainter *pPainter,
                                const QStyleOptionGraphicsItem *,
                                QWidget *)
{
    pPainter->setRenderHints(QPainter::Antialiasing |
                             QPainter::HighQualityAntialiasing);

    pPainter->setPen(m_colorPen);
    pPainter->setBrush(m_colorBackground);

//    QPainterPath painterPath = m_painterPath;
//    QPainterPathStroker painterStroker;
//    painterPath.addPath(painterStroker.createStroke(m_painterPath));
    pPainter->drawPath(m_painterPath);
    pPainter->setPen(Qt::NoPen);
    pPainter->setBrush(m_colorInOut);
    pPainter->drawPath(m_painterPath.subtracted(m_painterPathTrim));

//    pPainter->setBrush(m_colorPen);
    QPen pen(Qt::yellow);
    pen.setWidth(ValueSliderWidth);
    pPainter->setPen(pen);
    pPainter->setClipPath(m_painterPath);
    pPainter->drawLine(QPointF(m_rectF.width()*m_position, m_rectF.top()),
                       QPointF(m_rectF.width()*m_position, m_rectF.bottom()));
}

void QGraphicsTrimSlider::createObjects()
{
}

void QGraphicsTrimSlider::initObjects()
{
    setGeometry(0, 0, Width, Height);
}

void QGraphicsTrimSlider::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_position = event->pos().x();
    m_position /= m_rectF.width();
    setValue(m_position * (float)m_maxValue);
}

void QGraphicsTrimSlider::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    m_position = event->pos().x();

    if (m_position < 0)
        m_position = 0;
    else if (m_position > m_rectF.width())
        m_position = 1.0;
    else
        m_position /= m_rectF.width();

    setValue(m_position * (float)m_maxValue);
}

void QGraphicsTrimSlider::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    m_rectF.setSize(event->newSize());
    updatePaths();
}

QPainterPath QGraphicsTrimSlider::shape() const
{
    return m_painterPath;
}

QGraphicsTrimSlider::QGraphicsTrimSlider(QGraphicsItem */*parent*/) :
    m_colorInOut(ColorDarkGray, ColorDarkGray, ColorDarkGray),
    m_colorBackground(ColorLightGray, ColorLightGray, ColorLightGray),
    m_colorPen(ColorDarkGray, ColorDarkGray, ColorDarkGray),
    m_rectF(QRectF()),
    m_painterPath(QPainterPath()),
    m_value(MinimumValue),
    m_maxValue(MaximumValue),
    m_trimIn(MinimumValue),
    m_trimOut(MaximumValue),
    m_position(MinimumValue)
{
    initObjects();
}

QGraphicsTrimSlider::~QGraphicsTrimSlider()
{
}

void QGraphicsTrimSlider::setTrim(long in, long out)
{
    if ((in <= out) && !((m_trimIn == in) && (m_trimOut == out)))
    {
        m_trimIn = in;
        m_trimOut = out;
        updatePaths();
        update();
        emit signal_trimChanged(ffTrim(in,out));
    }
}

void QGraphicsTrimSlider::setTrim(ffTrim trim)
{
    setTrim(trim.m_in, trim.m_out);
}

void QGraphicsTrimSlider::setTrimIn(long in)
{
    setTrim(in, m_trimOut);
}

void QGraphicsTrimSlider::setTrimOut(long out)
{
    setTrim(m_trimIn, out);
}

void QGraphicsTrimSlider::setMaximum(long max)
{
    if ((max != m_maxValue) && (max >= m_minValue))
    {
        if (max < m_value)
            setValue(max);
        if (max < m_trimOut)
            setTrimOut(max);

        m_maxValue = max;
    }
}

void QGraphicsTrimSlider::setMinimum(long min)
{
    if ((min != m_minValue) && (min <= m_maxValue))
    {
        if (min > m_value)
            setValue(min);
        if (min > m_trimIn)
            setTrimIn(min);

        m_minValue = min;
    }
}

void QGraphicsTrimSlider::setValue(long value)
{
    if ((value != m_value) && ((value >= m_minValue) && (value <= m_maxValue)))
    {
        m_value = value;
        update();
        emit signal_valueChanged(m_value);
    }
}

long QGraphicsTrimSlider::maximum()
{
    return m_maxValue;
}

long QGraphicsTrimSlider::minimum()
{
    return m_minValue;
}

long QGraphicsTrimSlider::value()
{
    return m_value;
}

ffTrim QGraphicsTrimSlider::getTrim()
{
    ffTrim trim(m_trimIn, m_trimOut);

    return trim;
}

void QGraphicsTrimSlider::updatePaths()
{
    m_painterPath = QPainterPath();
//    m_painterPath.addRoundedRect(m_rectF, m_rectF.height()/m_rectF.width() *
//                                 100.0, 100.0, Qt::RelativeSize);
    m_painterPath.addRect(m_rectF);

    m_painterPathTrim = QPainterPath();

    qreal left, width;

    if (m_maxValue != 0)
    {
        left = ((qreal)(m_trimIn - 1)/(qreal)m_maxValue) * m_rectF.width();
        width = ((qreal)(m_trimOut - (m_trimIn - 1))/(qreal)m_maxValue) *
                m_rectF.width();
    }
    else
    {
        left = 1;
        width = m_rectF.width();
    }

    m_painterPathTrim.addRect(left, 0, width, m_rectF.height());
}
