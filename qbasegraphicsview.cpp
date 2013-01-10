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

#include "qbasegraphicsview.h"

QBaseGraphicsView::QBaseGraphicsView(QWidget *parent,
                                     QBaseGraphicsView *pPeer)
    : QGraphicsView(parent), m_pPeer(pPeer), m_activesState(ActivesOff)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("background:darkGray");
}

void QBaseGraphicsView::enterEvent(QEvent *event)
{
    QGraphicsView::enterEvent(event);
    viewport()->setCursor(Qt::CrossCursor);
}

void QBaseGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    viewport()->setCursor(Qt::CrossCursor);
}

void QBaseGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void QBaseGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    viewport()->setCursor(Qt::CrossCursor);
}

void QBaseGraphicsView::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}

bool QBaseGraphicsView::eventFilter(QObject *target, QEvent *event)
{
    QMouseEvent *me = static_cast<QMouseEvent*>(event);
    switch (event->type())
    {
    case QEvent::MouseButtonPress:
        if (m_activesState == ActivesOff)
        {
            if (!m_Actives.contains(itemAt(me->pos())))
            {
                if (m_pPeer != NULL)
                    m_pPeer->mousePressEvent(me);
                return true;
            }
            else
            {
                m_activesState = ActivesOn;
            }
        }
        break;
    case QEvent::MouseButtonRelease:
        if (m_activesState == ActivesOff)
        {
            if (m_pPeer != NULL)
                m_pPeer->mouseReleaseEvent(me);
            return true;
        }
        else
        {
            m_activesState = ActivesOff;
        }
        break;
    case QEvent::MouseMove:
        if (m_activesState == ActivesOff)
        {
            if (m_pPeer != NULL)
                m_pPeer->mouseMoveEvent(me);
            return true;
        }
        break;
    default:
        break;
    }
    return QGraphicsView::eventFilter(target, event);
}

QHash<QGraphicsItem *, bool>::iterator
QBaseGraphicsView::addActive(QGraphicsItem *pGI)
{
    return m_Actives.insert(pGI, true);
}


