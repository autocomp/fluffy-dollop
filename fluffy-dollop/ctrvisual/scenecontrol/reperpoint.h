#ifndef REPERPOINT_H
#define REPERPOINT_H

//#include <QAbstractGraphicsShapeItem>
//#include <QPainter>
//#include <QTimer>
//#include <QColor>

//namespace SceneControl
//{
//class ReperPoint : public QObject, public QAbstractGraphicsShapeItem
//{
//    Q_OBJECT

//    QTimer m_timer;
//    QColor m_defColor, m_selColor;

//public:
//    ReperPoint();
//    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
//    virtual QRectF boundingRect()const {return QRectF(-6,-6,15,15);}
//    virtual int type() const {return 777777;}
//    QPointF getFramePos();
//    void setFramePos(QPointF framePos);
//    void setGeoPos(QPointF wgsPos);
//    QPointF getGeoPos();
//    void setDefaultColor(QColor color);
//    void setSelectColor(QColor color);
//    void emit_recalkErrors() {emit signal_recalkErrors();}

//protected:
//    virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);

//private slots:
//    void slot_selectionChanged();

//signals:
//    void signal_itemSelectedChanged(bool);
//    void signal_itemPosChanged();
//    void signal_sceneToGeo(QPointF&);
//    void signal_sceneToFrame(QPointF&);
//    void signal_geoToScene(QPointF&);
//    void signal_frameToScene(QPointF&);
//    void signal_recalkErrors();
//};
//}
#endif
