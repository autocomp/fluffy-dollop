#ifndef SEARCHRECTGRAPHICSITEM_H
#define SEARCHRECTGRAPHICSITEM_H

#include <QGraphicsRectItem>
#include <QObject>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QPen>
#include <QBrush>

/**
 * @brief Класс для выделения области поиска
 */
class SearchRectGraphicsItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    QPen m_pen;
    QBrush m_brush;
    bool m_selectingFinished;
    QRectF m_rect;
    quint64 m_browserId;

public:
    explicit SearchRectGraphicsItem(quint64 btowserId, QGraphicsRectItem * gParent = 0, QObject *parent = 0);
    void setPoly(QPolygonF &rect);
    void finishSelecting(bool finish);
    int type(){return QGraphicsItem::UserType + 2003;}
    bool isSelectingFinished();

signals:
    void signalSelectingFinished(QRectF rect, quint64 id);

public slots:

};

#endif // SEARCHRECTGRAPHICSITEM_H
