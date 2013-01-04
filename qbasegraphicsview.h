#ifndef QBASEGRAPHICSVIEW_H
#define QBASEGRAPHICSVIEW_H

#include <QtGui>

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
