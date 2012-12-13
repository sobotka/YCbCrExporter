#ifndef TEXTPILL_H
#define TEXTPILL_H

#include <QGraphicsItem>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#define DEFAULT_TEXTCOLOR               QColor(220,220,220)
#define DEFAULT_FILLCOLOR               QColor(80,80,80)
class TextPill : public QGraphicsItem
{
private:
    QColor                  m_colorFill;
    QColor                  m_colorText;
    QString                 m_text;
    QGraphicsOpacityEffect *m_pEffect;
    QPropertyAnimation     *m_pPropertyAnimation;

    void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option, QWidget *);
    int roundness(double size) const;
    QRectF outlineRect(void) const;
    QRectF boundingRect() const;
public:
    TextPill(void);
    ~TextPill();

    void setText(const QString &text);
    void doFadeOut(void);

};

#endif // TEXTPILL_H
