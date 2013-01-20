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

void QTextPill::init(QString text, bool inverse, int dur, qreal start,
                     qreal end)
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
