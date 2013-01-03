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

#define MARGIN_WIDTH_FACTOR             3.0
#define MARGIN_HEIGHT_FACTOR            2.0

class QTextPill : public QGraphicsWidget
{
private:
    QColor                  m_colorFill;
    QColor                  m_colorText;
    QString                 m_text;

    QGraphicsOpacityEffect *m_pEffect;
    QPropertyAnimation     *m_pPropertyAnimation;


    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    void createObjects(void);
    void initObjects(void);

    QRectF boundingRect() const;

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
