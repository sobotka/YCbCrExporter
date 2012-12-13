#include "textpill.h"
#include <QPainter>
#include <QApplication>

TextPill::TextPill(void) :
    m_colorFill(DEFAULT_FILLCOLOR), m_colorText(DEFAULT_TEXTCOLOR), m_text(""),
    m_pEffect(NULL), m_pPropertyAnimation(NULL)
{
}

TextPill::~TextPill()
{
    delete m_pEffect;
    m_pEffect = NULL;
    delete m_pPropertyAnimation;
    m_pPropertyAnimation = NULL;
}

void TextPill::paint(QPainter *pPainter,
                 const QStyleOptionGraphicsItem *,
                 QWidget *)
{
    pPainter->setRenderHint(QPainter::Antialiasing);

    pPainter->setPen(Qt::NoPen);
    pPainter->setBrush(m_colorFill);

    QRectF rect = outlineRect();
    pPainter->drawRoundRect(rect, rect.height()/rect.width() * 100,
                            100.0);

    pPainter->setPen(m_colorText);
    pPainter->drawText(rect, Qt::AlignCenter, m_text);
}

void TextPill::setText(const QString &text)
{
    prepareGeometryChange();
    m_text = text;
    update();
}

QRectF TextPill::outlineRect(void) const
{
    QFontMetricsF metrics = QApplication::font();
    QRectF rect = metrics.boundingRect(m_text);
    rect.adjust(-metrics.height(), -metrics.height()/2, +metrics.height(), +metrics.height()/2);
    rect.translate(-rect.center());
    return rect;
}

QRectF TextPill::boundingRect() const
{
    const int Margin = 1;
    return outlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}

