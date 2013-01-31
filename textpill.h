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

#ifndef TEXTPILL_H
#define TEXTPILL_H

#include <QtGui>

#define DEFAULT_TEXTCOLOR               QColor(220,220,220)
#define DEFAULT_FILLCOLOR               QColor(40,40,40)

#define DEFAULT_DURATION                3000
#define DEFAULT_TEXT                    "Only a test"
#define DEFAULT_STARTVALUE              1.0
#define DEFAULT_ENDVALUE                0.0
#define DEFAULT_STEP                    0.0
#define DEFAULT_STEPVALUE               1.0

#define WIDTHFACTOR                     3.0
#define HEIGHTFACTOR                    2.0

class QTextPill : public QGraphicsWidget
{
private:
    QColor                  m_colorFill;
    QColor                  m_colorText;
    QString                 m_text;
    QFontMetricsF           m_metrics;
    QRectF                  m_rectF;
    //QRectF                  m_oldRectF;

    QGraphicsOpacityEffect *m_pEffect;
    QPropertyAnimation     *m_pPropertyAnimation;


    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    void createObjects(void);
    void initObjects(void);

    QRectF boundingRect() const;
    //QPainterPath shape() const;

    void setText(QString text);
public:
    QTextPill(QGraphicsItem *parent = 0);
    ~QTextPill();

    void start(QString text, bool inverse = false);
    void init(QString text = DEFAULT_TEXT, bool inverse = false,
              int dur = DEFAULT_DURATION, qreal start = DEFAULT_STARTVALUE,
              qreal end = DEFAULT_ENDVALUE);
};

#endif // TEXTPILL_H
