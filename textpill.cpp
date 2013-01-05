#include "textpill.h"
#include <QPainter>
#include <QApplication>
#include <QGraphicsScene>

#include <stdio.h>

QTextPill::QTextPill(QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    m_colorFill(DEFAULT_FILLCOLOR), m_colorText(DEFAULT_TEXTCOLOR), m_text(""),
    m_metrics(QApplication::font()), m_rectF(), m_pEffect(NULL),
    m_pPropertyAnimation(NULL)
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
    if (m_pPropertyAnimation->state() == QPropertyAnimation::Running)
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

    //prepareGeometryChange();

    QRectF rect = boundingRect();

    pPainter->drawRoundRect(rect, rect.height()/rect.width() * 100.0,
                            100.0);

    pPainter->setPen(m_colorText);
    pPainter->drawText(rect, Qt::AlignCenter, m_text);
}

void QTextPill::setText(QString text)
{
    m_text = text;

    m_rectF.setRect(0, 0,
                    m_metrics.boundingRect(m_text).width() +
                    WIDTHFACTOR *
                    m_metrics.boundingRect(m_text).height(),
                    HEIGHTFACTOR *
                    m_metrics.boundingRect(m_text).height());
}

QRectF QTextPill::boundingRect() const
{
    return m_rectF;
}

/* QPainterPath QTextPill::shape() const
{
    QPainterPath path;
    QRectF rect = boundingRect();
    path.addRoundedRect(rect, rect.height()/rect.width() * 100.0,
                                100.0);
    return path;
} */
