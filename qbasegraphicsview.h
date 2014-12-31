/*******************************************************************************
YCbCr Exporter
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

#ifndef QBASEGRAPHICSVIEW_H
#define QBASEGRAPHICSVIEW_H

#include <QtWidgets>

class QBaseGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    enum ActivesState
    {
        ActivesOff, ActivesOn
    };
private:
    QBaseGraphicsView                      *m_pPeer;
    QHash<QGraphicsItem *, bool>            m_Actives;

    ActivesState                            m_activesState;
public:
    explicit QBaseGraphicsView(QWidget *parent = 0,
                               QBaseGraphicsView *pPeer = NULL);

    void enterEvent(QEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);

    QHash<QGraphicsItem *, bool>::iterator addActive(QGraphicsItem *);

    bool eventFilter(QObject *, QEvent *);

signals:
    
public slots:
    
};

#endif // QBASEGRAPHICSVIEW_H
