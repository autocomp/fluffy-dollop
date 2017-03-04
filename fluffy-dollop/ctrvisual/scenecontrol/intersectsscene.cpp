#include "intersectsscene.h"
#include <QGraphicsPolygonItem>
#include <QDebug>

//#include <boost/geometry.hpp>
//#include <boost/geometry/geometries/point_xy.hpp>
//#include <boost/geometry/geometries/polygon.hpp>
//#include <boost/geometry/multi/geometries/multi_polygon.hpp>
//#include <boost/geometry/geometries/register/point.hpp>
//#include <boost/geometry/geometries/register/ring.hpp>
////#include <boost/geometry/multi/algorithms/transform.hpp>

//BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(QPointF, double, cs::cartesian, x, y, setX, setY)
//BOOST_GEOMETRY_REGISTER_RING(QPolygonF)

using namespace visualize_system;

IntersectsScene::IntersectsScene()
    : Z_LEVEL_FOR_TOP_RASTER(1000000000)
    , _itemOnTop(0)
{
}

void IntersectsScene::addPolygon(uint id, QPolygonF scenePolygon, bool isBaseCover)
{
    scenePolygon.append(scenePolygon.first());
    QGraphicsPolygonItem* item = new QGraphicsPolygonItem(scenePolygon);
    addItem(item);
    item->setData(ID, id);
    item->setData(IS_BASE_COVER, isBaseCover);
    item->setData(TRANSPARENCY, 0);
    item->setData(VISIBILITY, true);
    if(isBaseCover)
    {
        auto itInsertBefore = _itemsByZlevel.end();
        int counter(0);
        for(auto it = _itemsByZlevel.begin(); it != _itemsByZlevel.end(); ++it)
        {
            if(itInsertBefore != _itemsByZlevel.end())
            {
                (*it)->setZValue(++counter);
            }
            else if( (*it)->data(IS_BASE_COVER).toBool() == false )
            {
                // это первое НЕ базовое покрытие, нужно вставить базовое перед ним
                itInsertBefore = it;
                counter = (*it)->zValue();
                item->setZValue(counter);
                ++counter;
                (*it)->setZValue(counter);
            }
        }

        if(itInsertBefore != _itemsByZlevel.end())
            _itemsByZlevel.insert(itInsertBefore, item);
        else
        {
            // либо это вообще первое покрыти, либо все покрытия базовые.
            item->setZValue( _itemsByZlevel.isEmpty() ? 0 : ( _itemsByZlevel.last()->zValue() + 1 ) );
            _itemsByZlevel.append(item);
        }
    }
    else
    {
        item->setZValue( _itemsByZlevel.isEmpty() ? 0 : ( _itemsByZlevel.last()->zValue() + 1 ) );
        _itemsByZlevel.append(item);
    }

//    qDebug() << "-----------";
//    foreach(QGraphicsPolygonItem* item, _itemsByZlevel)
//    {
//        const uint id(item->data(ID).toUInt());
//        bool isBaseCover(item->data(IS_BASE_COVER).toBool());
//        qDebug() << "id" << id << "isBaseCover" << isBaseCover << "zValue" << QString::number(item->zValue(),'f',2);
//    }
//    qDebug() << "-----------";
}

void IntersectsScene::setZvalue(uint providerId, uint zValue)
{
    QGraphicsPolygonItem* foundProvider = 0;
    for(auto it = _itemsByZlevel.begin(); it != _itemsByZlevel.end(); ++it)
    {
        const uint id((*it)->data(ID).toUInt());
        if(id == providerId)
        {
            foundProvider = *it;
            foundProvider->setZValue(zValue);
            _itemsByZlevel.erase(it);
            break;
        }
    }
    if(foundProvider)
    {
        uint zCounter = 0;
        QList<QGraphicsPolygonItem*> itemsByZlevel;
        foreach(QGraphicsPolygonItem* item, _itemsByZlevel)
        {
            if(zCounter == zValue)
            {
                foundProvider->setZValue(zCounter);
                itemsByZlevel.append(foundProvider);
                foundProvider = 0;
                ++zCounter;
            }

            item->setZValue(zCounter);
            itemsByZlevel.append(item);
            ++zCounter;
        }
        _itemsByZlevel = itemsByZlevel;
        if(foundProvider)
            _itemsByZlevel.append(foundProvider);
    }

//    qDebug() << "--- IntersectsScene::setZvalue, providerId:" << providerId << ", zValue:" << zValue;
//    foreach(QGraphicsPolygonItem* item, _itemsByZlevel)
//    {
//        const uint id(item->data(ID).toUInt());
//        bool isBaseCover(item->data(IS_BASE_COVER).toBool());
//        qDebug() << "id" << id << "isBaseCover" << isBaseCover << "zValue" << QString::number(item->zValue(),'f',2);
//    }
//    qDebug() << "-----------";
}

void IntersectsScene::setPolygon(uint id, QPolygonF scenePolygon)
{
    foreach (QGraphicsPolygonItem* polygon, _itemsByZlevel)
        if(polygon->data(ID).toUInt() == id)
        {
            polygon->setPolygon(scenePolygon);
            qDebug() << "IntersectsScene::setPolygon for " << id << ", scenePolygon:" << scenePolygon;
            break;
        }
}

void IntersectsScene::removePolygon(uint id)
{
    // все нужно переделать с учетом верхнего слоя и его REAL_Z_VALUE.

    if(id == _itemOnTop)
        _itemOnTop = 0;

    int zLevel = 0;
    auto itForRemove(_itemsByZlevel.end());
    for(auto it = _itemsByZlevel.begin(); it != _itemsByZlevel.end(); ++it)
    {
        QGraphicsPolygonItem* item = *it;
        if(item->data(ID).toUInt() == id)
        {
            itForRemove = it;
        }
        else if(itForRemove != _itemsByZlevel.end())
        {
            item->setZValue(zLevel);
            ++zLevel;
        }
    }
    if(itForRemove != _itemsByZlevel.end())
    {
        delete (*itForRemove);
        _itemsByZlevel.erase(itForRemove);
    }
}

void IntersectsScene::getProvidersInTile(const QRectF & tileSceneRect, QList<uint> & baseProviders, QList<uint> & otherProviders, uint maxRastersInArea)
{
    QList<QGraphicsItem*> itemsList = items(tileSceneRect, Qt::IntersectsItemShape, Qt::DescendingOrder);
    QPolygonF sceneRectPolygon(tileSceneRect);
    int rastersInArea(0);
    bool hideItemOnTop(false);
    foreach (QGraphicsItem* item, itemsList)
    {
        QGraphicsPolygonItem* polItem = dynamic_cast<QGraphicsPolygonItem*>(item);
        if(polItem)
        {
            const uint id(item->data(ID).toUInt());
            if(polItem->data(IS_BASE_COVER).toBool() == false)
            {
                if(rastersInArea < maxRastersInArea)
                {
                    QPolygonF interPol = polItem->polygon().intersected(sceneRectPolygon);
                    if(interPol.isEmpty() == false)
                    {
                        const uint transparency(polItem->data(TRANSPARENCY).toUInt());
                        if(transparency >= 95 || polItem->data(VISIBILITY).toBool() == false)
                        {
                            if(id == _itemOnTop)
                                hideItemOnTop = true;
                            continue;
                        }

                        if(transparency <= 5)
                        {
                            ++rastersInArea;
                        }
                        otherProviders.prepend(id);
                    }
                }
            }
            else // IS_BASE_COVER
            {
                const uint transparency(polItem->data(TRANSPARENCY).toUInt());
                if(transparency >= 95 || polItem->data(VISIBILITY).toBool() == false)
                    continue;

                QPolygonF interPol = polItem->polygon().intersected(sceneRectPolygon);
                if(interPol.isEmpty() == false)
                    baseProviders.prepend(id);
            }
        }
    }

    if(_itemOnTop > 0 && hideItemOnTop == false)
    {
        for(auto it = otherProviders.begin(); it != otherProviders.end(); ++it)
            if ( (*it) == _itemOnTop )
            {
                otherProviders.erase(it);
                break;
            }
        otherProviders.append(_itemOnTop);
    }
}

QList<uint> IntersectsScene::check(const QRectF & sceneRect, bool BASE_COVER_ONLY, uint maxRastersInArea)
{
    QList<QGraphicsItem*> itemsList = items(sceneRect, Qt::IntersectsItemShape, Qt::DescendingOrder);
    QList<uint> idList;

    QPolygonF sceneRectPolygon(sceneRect);
    int rastersInArea(0);
    bool hideItemOnTop(false);
    foreach (QGraphicsItem* item, itemsList)
    {
        QGraphicsPolygonItem* polItem = dynamic_cast<QGraphicsPolygonItem*>(item);
        if(polItem)
        {
            const uint id(item->data(ID).toUInt());
            if(polItem->data(IS_BASE_COVER).toBool() == false)
            {
                if(BASE_COVER_ONLY == false)
                {
                    if(rastersInArea < maxRastersInArea)
                    {
                        QPolygonF interPol = polItem->polygon().intersected(sceneRectPolygon);
                        if(interPol.isEmpty() == false)
                        {
                            const uint transparency(polItem->data(TRANSPARENCY).toUInt());
                            if(transparency >= 95 || polItem->data(VISIBILITY).toBool() == false)
                            {
                                if(id == _itemOnTop)
                                    hideItemOnTop = true;
                                continue;
                            }

                            if(transparency <= 5)
                            {
//                                bool areaInPolygon(true);
//                                foreach(QPointF p, sceneRectPolygon)
//                                    if(polItem->polygon().containsPoint(p, Qt::OddEvenFill) == false)
//                                    {
//                                        areaInPolygon = false;
//                                        break;
//                                    }

//                                if(areaInPolygon)
//                                {
//                                    rastersInArea = maxRastersInArea;
//                                    // проблема !- если так сделать, мы потеряем _itemOnTop, если он ниже !

//                                }
//                                else
                                    ++rastersInArea;
                            }
                            idList.append(id);
                        }
                    }
                }
            }
            else // IS_TMS
            {
                const uint transparency(polItem->data(TRANSPARENCY).toUInt());
                if(transparency >= 95 || polItem->data(VISIBILITY).toBool() == false)
                    continue;

                QPolygonF interPol = polItem->polygon().intersected(sceneRectPolygon);
                if(interPol.isEmpty() == false)
                    idList.append(id);
            }
        }
    }

    if(_itemOnTop > 0 && hideItemOnTop == false)
    {
        for(auto it = idList.begin(); it != idList.end(); ++it)
            if ( (*it) == _itemOnTop )
            {
                idList.erase(it);
                break;
            }
        idList.prepend(_itemOnTop);
    }

    return idList;
}

QList<uint> IntersectsScene::check(const QPointF & scenePos)
{
    QList<QGraphicsItem*> itemsList = items(scenePos, Qt::IntersectsItemShape, Qt::DescendingOrder);
    QList<uint> idList;
    foreach (QGraphicsItem* item, itemsList)
    {
        QGraphicsPolygonItem* polItem = dynamic_cast<QGraphicsPolygonItem*>(item);
        if(polItem)
        {
            const uint id(item->data(ID).toUInt());
            if(polItem->data(IS_BASE_COVER).toBool() == false)
            {
                const uint transparency(polItem->data(TRANSPARENCY).toUInt());
                if(transparency >= 95 || polItem->data(VISIBILITY).toBool() == false)
                    continue;

                if(polItem->polygon().containsPoint(scenePos, Qt::OddEvenFill))
                    idList.append(id);
            }
        }
    }
    return idList;
}

void IntersectsScene::setProviderOnTop(uint id)
{
    _itemOnTop = id;

//    if(_itemOnTop.isEmpty() == false)
//    {
//        foreach (QGraphicsPolygonItem* item, _itemsByZlevel)
//        {
//            const QString name(item->data(NAME).toString());
//            if(name == _itemOnTop)
//            {
//                QVariant var = item->data(REAL_Z_VALUE);
//                if(var.isValid())
//                {
//                    item->setZValue(var.toUInt());
//                    item->setData(REAL_Z_VALUE, QVariant());
//                }
//                break;
//            }
//        }
//    }

//    _itemOnTop = provName;

//    foreach (QGraphicsPolygonItem* item, _itemsByZlevel)
//    {
//        const QString name(item->data(NAME).toString());
//        if(name == _itemOnTop)
//        {
//            item->setData(REAL_Z_VALUE, item->zValue());
//            item->setZValue(Z_LEVEL_FOR_TOP_RASTER);
//            break;
//        }
//    }
}

void IntersectsScene::setTransparency(uint id, uint persents)
{
    foreach (QGraphicsPolygonItem* polygon, _itemsByZlevel)
        if(polygon->data(ID).toUInt() == id)
        {
            polygon->setData(TRANSPARENCY, persents);
        }
}

void IntersectsScene::setVisibility(uint id, bool on_off)
{
    foreach (QGraphicsPolygonItem* polygon, _itemsByZlevel)
        if(polygon->data(ID).toUInt() == id)
        {
            polygon->setData(VISIBILITY, on_off);
        }
}

void IntersectsScene::clearScene()
{
    foreach (QGraphicsPolygonItem* polygon, _itemsByZlevel)
        delete polygon;
    _itemsByZlevel.clear();
    _itemOnTop = 0;
}


/*
QList<uint> IntersectsScene::check(const QRectF & sceneRect, uint maxRastersInArea)
{
    // метод нужно бужет принципиально переделать при разделении на базовые слои и слои для обработки !

    QList<QGraphicsItem*> itemsList = items(sceneRect, Qt::IntersectsItemShape, Qt::DescendingOrder);
    QStringList nameList;

    using polygon_type = boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> >;
    using multi_polygon_type = boost::geometry::model::multi_polygon<polygon_type>;

    QPolygonF sceneRectPolygon(sceneRect);
    std::reverse(sceneRectPolygon.begin(), sceneRectPolygon.end());
    multi_polygon_type mp_tile;
    boost::geometry::intersection(sceneRectPolygon, sceneRectPolygon, mp_tile);
    //boost::geometry::correct()
    multi_polygon_type mp_res;


    double tileArea = boost::geometry::area(sceneRectPolygon);
    int rastersInArea(0);
    foreach (QGraphicsItem* item, itemsList)
    {
        QGraphicsPolygonItem* polItem = dynamic_cast<QGraphicsPolygonItem*>(item);
        if(polItem)
        {
            const QString name(item->data(NAME).toString());

            if(polItem->data(IS_RASTER).toBool())
            {
                if(rastersInArea < maxRastersInArea)
                {
                    QPolygonF interPol = polItem->polygon().intersected(sceneRectPolygon);
                    if(interPol.isEmpty() == false)
                    {
                        const uint transparency(polItem->data(TRANSPARENCY).toUInt());
                        if(transparency >= 95 || polItem->data(VISIBILITY).toBool() == false)
                            continue;

                        if(transparency <= 5)
                        {

                            multi_polygon_type p;
                            boost::geometry::intersection(polItem->polygon(), polItem->polygon(), p);

                            multi_polygon_type mp_tmp;
                            boost::geometry::intersection(mp_tile, p, mp_tmp);
                            multi_polygon_type mp_tmp2;
                            boost::geometry::union_(mp_res, mp_tmp, mp_tmp2);

                            double prev_area = boost::geometry::area(mp_res);
                            double new_area = boost::geometry::area(mp_tmp2);

                            if (prev_area < new_area)
                            {
                                mp_res = mp_tmp2;
                                nameList.append(name);
                                ++rastersInArea;
                            }

                            if (std::abs(new_area - tileArea) < 0.001)
                            {
                                rastersInArea = maxRastersInArea;
                            }



//                            bool areaInPolygon(true);
//                            foreach(QPointF p, sceneRectPolygon)
//                                if(polItem->polygon().containsPoint(p, Qt::OddEvenFill) == false)
//                                {
//                                    areaInPolygon = false;
//                                    break;
//                                }

//                            if(areaInPolygon)
//                                rastersInArea = maxRastersInArea;
//                            else
//                                ++rastersInArea;

                        }
                        else
                        {
                            ++rastersInArea;
                            nameList.append(name);
                        }
                    }
                }
            }
            else // IS_TMS
            {
                QPolygonF interPol = polItem->polygon().intersected(sceneRectPolygon);
                if(interPol.isEmpty() == false)
                    nameList.append(name);
            }
        }
    }

    return nameList;
}
*/

/*
QList<uint> IntersectsScene::check(const QRectF & sceneRect, bool TMS_ONLY, uint maxRastersInArea)
{
    // метод нужно бужет принципиально переделать при разделении на базовые слои и слои для обработки !

    QList<QGraphicsItem*> itemsList = items(sceneRect, Qt::IntersectsItemShape, Qt::DescendingOrder);
    QList<uint> idList;

    QPolygonF sceneRectPolygon(sceneRect);
    int rastersInArea(0);
    // создаем область равную тайлу на сцене ---> sceneRect

    using polygon_type = boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> >;
    using multi_polygon_type = boost::geometry::model::multi_polygon<polygon_type>;

    boost::geometry::correct(sceneRectPolygon);
    double tileArea = boost::geometry::area(sceneRectPolygon);

    multi_polygon_type mpTile;
    boost::geometry::intersection(sceneRectPolygon, sceneRectPolygon, mpTile);

    multi_polygon_type mpResult;

    foreach (QGraphicsItem* item, itemsList)
    {
        QGraphicsPolygonItem* polItem = dynamic_cast<QGraphicsPolygonItem*>(item);
        if(polItem)
        {
            const uint id(item->data(ID).toUInt());
            if(polItem->data(IS_RASTER).toBool())
            {
                if(TMS_ONLY == false)
                {
                    if(rastersInArea < maxRastersInArea)
                    {
                        QPolygonF interPol = polItem->polygon().intersected(sceneRectPolygon);
                        if(interPol.isEmpty() == false)
                        {
                            const uint transparency(polItem->data(TRANSPARENCY).toUInt());
                            if(transparency >= 95 || polItem->data(VISIBILITY).toBool() == false)
                            {
                                continue;
                            }

                            if(transparency <= 5)
                            {
                                // вычитаем из области текущий растр --->  sceneRectPolygon -= interPol

                                boost::geometry::correct(interPol);

                                multi_polygon_type p;
                                boost::geometry::intersection(interPol, interPol, p);

                                multi_polygon_type mpTempInTile;
                                boost::geometry::intersection(mpTile, p, mpTempInTile);
                                multi_polygon_type mpCurrent;
                                boost::geometry::union_(mpResult, mpTempInTile, mpCurrent);

                                double prev_area = boost::geometry::area(mpResult);
                                double new_area = boost::geometry::area(mpCurrent);

                                if(prev_area < new_area)
                                {
                                    idList.append(id); // add
                                    mpResult = mpCurrent;
                                }

                                if (std::abs(tileArea - new_area) < 1e-8)
                                {
                                    rastersInArea = maxRastersInArea;
                                }


//                                bool areaInPolygon(true);
//                                foreach(QPointF p, sceneRectPolygon)
//                                    if(polItem->polygon().containsPoint(p, Qt::OddEvenFill) == false)
//                                    {
//                                        areaInPolygon = false;
//                                        break;
//                                    }

//                                if(areaInPolygon)
//                                    rastersInArea = maxRastersInArea;
//                                else
//                                    ++rastersInArea;
                            }
                            else
                            {
                                idList.append(id);
                            }
                            // проверяем не пустая ли область тайла

//                            idList.append(id); // add
                        }
                    }
                }
            }
            else // IS_TMS
            {
                const uint transparency(polItem->data(TRANSPARENCY).toUInt());
                if(transparency >= 95 || polItem->data(VISIBILITY).toBool() == false)
                    continue;

                QPolygonF interPol = polItem->polygon().intersected(sceneRectPolygon);
                if(interPol.isEmpty() == false)
                    idList.append(id);
            }
        }
    }

    return idList;
}
*/

































