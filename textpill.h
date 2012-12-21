#ifndef TEXTPILL_H
#define TEXTPILL_H

#include <QGraphicsItem>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#define DEFAULT_TEXTCOLOR               QColor(220,220,220)
#define DEFAULT_FILLCOLOR               QColor(40,40,40)

#define DEFAULT_DURATION                2000
#define DEFAULT_TEXT                    "Only a test"
#define DEFAULT_STARTVALUE              1.0
#define DEFAULT_ENDVALUE                0.0
#define DEFAULT_STEP                    0.0
#define DEFAULT_STEPVALUE               1.0

#define MARGIN_WIDTH_FACTOR             3.0
#define MARGIN_HEIGHT_FACTOR            2.0

class QTextPill : public QGraphicsItem
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
    QTextPill(void);
    ~QTextPill();

    void start(void);
    void init(QString text = DEFAULT_TEXT, int dur = DEFAULT_DURATION,
              qreal start = DEFAULT_STARTVALUE, qreal end = DEFAULT_ENDVALUE);
};

#endif // TEXTPILL_H