#include "textpill.h"
#include <QPainter>
#include <QApplication>
#include <QGraphicsScene>

#include <stdio.h>

QTextPill::QTextPill(QGraphicsItem *parent) :
    QGraphicsItem(parent),
    m_colorFill(DEFAULT_FILLCOLOR), m_colorText(DEFAULT_TEXTCOLOR), m_text(""),
    m_pEffect(NULL), m_pPropertyAnimation(NULL)
{
    createObjects();
    initObjects();
}

QTextPill::~QTextPill()
{
    delete m_pPropertyAnimation;
    delete m_pEffect;
}

void QTextPill::createObjects(void)
{
    m_pEffect = new QGraphicsOpacityEffect;
    m_pPropertyAnimation = new QPropertyAnimation(m_pEffect, "opacity");
    setGraphicsEffect(m_pEffect);
}

void QTextPill::initObjects(void)
{
    init();
}

void QTextPill::init(QString text, bool inverse, int dur, qreal start, qreal end)
{
    setOpacity(DEFAULT_ENDVALUE);
    setText(text);
    if (inverse)
        m_pPropertyAnimation->setDirection(QAbstractAnimation::Backward);
    else
        m_pPropertyAnimation->setDirection(QAbstractAnimation::Forward);
    m_pPropertyAnimation->setDuration(dur);
    m_pPropertyAnimation->setStartValue(start);
    m_pPropertyAnimation->setEndValue(end);
    m_pPropertyAnimation->setEasingCurve(QEasingCurve::InQuint);
}

void QTextPill::start(QString text, bool inverse)
{
    m_pPropertyAnimation->stop();
    init(text, inverse);
    setOpacity(DEFAULT_STARTVALUE);
    m_pPropertyAnimation->start();
}

void QTextPill::paint(QPainter *pPainter, const QStyleOptionGraphicsItem *,
                 QWidget *)
{
    pPainter->setRenderHints(QPainter::Antialiasing |
                             QPainter::TextAntialiasing |
                             QPainter::HighQualityAntialiasing);

    pPainter->setPen(Qt::NoPen);
    pPainter->setBrush(m_colorFill);

    QRectF rect = boundingRect();

    pPainter->drawRoundRect(rect, rect.height()/rect.width() * 100.0,
                            100.0);

    pPainter->setPen(m_colorText);
    pPainter->drawText(rect, Qt::AlignCenter, m_text);
}

void QTextPill::setText(QString text)
{
    prepareGeometryChange();
    m_text = text;
    update();
}

QRectF QTextPill::boundingRect() const
{
    QFontMetricsF metrics = QApplication::font();

    QRectF rect(0, 0,
                metrics.boundingRect(m_text).width() +
                MARGIN_WIDTH_FACTOR *
                metrics.boundingRect(m_text).height(),
                MARGIN_HEIGHT_FACTOR *
                metrics.boundingRect(m_text).height());
    return rect;
}

/*QPainterPath TextPill::shape() const
{
    QPainterPath path;
    QRectF rect = boundingRect();
    path.addRoundedRect(rect, rect.height()/rect.width() * 100,
                         100.0);
    return path;
}*/
