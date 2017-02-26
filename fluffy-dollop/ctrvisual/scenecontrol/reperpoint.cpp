//#include "reperpoint.h"
//#include <QDebug>

//using namespace SceneControl;

//ReperPoint::ReperPoint()
//    : m_timer(this)
//    , m_defColor(QColor(Qt::red))
//    , m_selColor(QColor(Qt::yellow))
//{
//    setFlags(QGraphicsItem::ItemIgnoresTransformations /*| QGraphicsItem::ItemIsMovable*/ | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsScenePositionChanges);
//    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slot_selectionChanged()), Qt::QueuedConnection);
//}

//void ReperPoint::setDefaultColor(QColor color)
//{
//    m_defColor = color;
//}

//void ReperPoint::setSelectColor(QColor color)
//{
//    m_selColor = color;
//}

//void ReperPoint::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
//{
//    painter->setPen(isSelected() ? m_selColor : m_defColor);
//    painter->drawLine(-5,0,5,0);
//    painter->drawLine(0,-5,0,5);
//}

//QVariant ReperPoint::itemChange(GraphicsItemChange change, const QVariant & value)
//{
//    QVariant var = QAbstractGraphicsShapeItem::itemChange(change, value);

//    switch(change)
//    {
//    case QGraphicsItem::ItemSelectedChange :
//        m_timer.start(10);
//        break;
//    case QGraphicsItem::ItemPositionChange :
////        emit signal_itemPosChanged();
//        break;
//    }

//    return var;
//}

//void ReperPoint::setGeoPos(QPointF wgsPos)
//{
//    emit signal_geoToScene(wgsPos);
//    setPos(wgsPos);
//}

//void ReperPoint::setFramePos(QPointF framePos)
//{
//    emit signal_frameToScene(framePos);
//    setPos(framePos);
//}

//QPointF ReperPoint::getFramePos()
//{
//    QPointF _pos(pos());
//    emit signal_sceneToFrame(_pos);
//    return _pos;
//}

//QPointF ReperPoint::getGeoPos()
//{
//    QPointF _pos(pos());
//    emit signal_sceneToGeo(_pos);
//    return QPointF(_pos.x(), _pos.y());
//}

//void ReperPoint::slot_selectionChanged()
//{
//    m_timer.stop();
//    emit signal_itemSelectedChanged(isSelected());
//}
