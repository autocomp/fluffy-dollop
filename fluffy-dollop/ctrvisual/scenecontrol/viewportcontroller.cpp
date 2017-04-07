#include "viewportcontroller.h"
#include "scenecontroller.h"
#include <QDebug>;
#include "math.h"
#include <ctrcore/visual/abstracttilemapper.h>

using namespace visualize_system;

ViewPortController::ViewPortController(SceneController * const sceneController)
    : QObject(sceneController)
    , m_SceneController(sceneController)
    , m_zoom(0)
    , m_leftX(0)
    , m_rightX(0)
    , m_topY(0)
    , m_bottomY(0)
    , m_chenged(false)
    , m_updateContent(false)
{
    connect(m_SceneController, SIGNAL(signal_recivedImage(const QPixmap&, const QString&)), this, SLOT(receivedImage(const QPixmap&, const QString&)));
    connect(m_SceneController, SIGNAL(loadingFinished()), this, SLOT(loadingFinished()));

//    QObject * myObject = new QObject;
//    myObject->moveToThread(QApplication::instance()->thread());

}

ViewPortController::~ViewPortController()
{
    QList<Cell> cellForDeleting;
    for(QMap<QString,Cell>::iterator it(m_cellMap.begin()); it != m_cellMap.end(); ++it)
        cellForDeleting.append(it.value());
    deleteCell(cellForDeleting);
}

void ViewPortController::update(int zoom, QRectF visibleRect, bool deleteUnvisibleCells)
{
//    double W(visibleRect.width()), H(visibleRect.height());
//    if(m_largeViewport)
//        r = QRectF( visibleRect.x() - W/2. ,  visibleRect.y() - H/2. , W * 2., H * 2.);
    localUpdate(zoom, visibleRect, deleteUnvisibleCells);
}

void ViewPortController::localUpdate(int zoom, QRectF visibleRect, bool deleteUnvisibleCells)
{
    m_viewOriginalRect = visibleRect;
    if(zoom <= 5)
    {
        QRectF _visibleRect(0, 0, m_SceneController->tileSize().width()*m_SceneController->tilesCount().width(), m_SceneController->tileSize().height()*m_SceneController->tilesCount().height());
        //qDebug() << "ViewController::update, _visibleRect:" << _visibleRect << ", visibleRect:" << visibleRect;
        visibleRect = visibleRect.intersected(_visibleRect);
    }

    QList<Cell> visibleCellList;

    if( getVisibleCells(zoom, visibleRect, visibleCellList) )
    {
//        {
//            //! сравнивая полученные ячейки с сохраненными с прошлого раза находим те ячейки которые вышли за перделы видимости и удаляем их
//            QList<Cell> cellsListForDeleting;

//            foreach(Cell cell, visibleCellList)
//            {
//                QMap<uint, Cell>::iterator it = m_cellMap.begin();
//                while (it != m_cellMap.end())
//                {
//                    if(it.value() == cell)
//                        break;
//                    ++it;
//                }
//                if(it != m_cellMap.end())
//                    cellsListForDeleting.append(cell);
//            }

//            //! удаляем невидимые ячейки
//            if(cellsListForDeleting.isEmpty() == false)
//                deleteCell(cellsListForDeleting);

//            //! отправляем запрос на обновление ячеек
//            if(visibleCellList.isEmpty() == false)
//                addCell(visibleCellList);

//            //! сохраняем видимые ячейки
//            foreach(Cell cell, visibleCellList)
//                m_cellMap.insert(cell.id, cell);
//        }


        QList<Cell> tempCellsList;
        QList<Cell> cellsListForAdding;
        foreach(Cell cell, visibleCellList)
        {
            QMap<QString, Cell>::iterator it = m_cellMap.begin();
            while (it != m_cellMap.end())
            {
                if(it.value() == cell)
                {
                    tempCellsList.append(it.value());
                    m_cellMap.erase(it);
                    break;
                }
                ++it;
            }
            if(it == m_cellMap.end())
            {
                cellsListForAdding.append(cell);
            }
        }

        if(deleteUnvisibleCells)
        {
            QList<Cell> cellsListForDeleting;
            QMap<QString, Cell>::iterator it = m_cellMap.begin();
            while (it != m_cellMap.end())
            {
                cellsListForDeleting.append(it.value());
                ++it;
            }
            if(cellsListForDeleting.isEmpty() == false)
                deleteCell(cellsListForDeleting);

            m_cellMap.clear();
        }

        foreach(Cell cell, cellsListForAdding)
            m_cellMap.insert(cell.toString(), cell);

        foreach(Cell cell, tempCellsList)
            m_cellMap.insert(cell.toString(), cell);

        if(cellsListForAdding.isEmpty() == false)
        {
//            if(m_SceneController->viewOptimization())
//                for(QMap<QString,Cell>::iterator it(m_cellMap.begin()); it != m_cellMap.end(); ++it)
//                    if(it.value().pBackgroundPixmapItem)
//                        it.value().pBackgroundPixmapItem->show();

            addCell(cellsListForAdding);
        }
    }
}


bool ViewPortController::getVisibleCells(int zoom, QRectF viewportRect, QList<Cell>& cellList)
{
    double POW(pow(2,zoom-1));
    double tileWidthOnView(m_SceneController->tileSize().width() / POW);
    double tileHeightOnView(m_SceneController->tileSize().height() / POW);
    int64_t MAX_WIDTH(m_SceneController->tilesCount().width()*POW);
    int64_t MAX_HEIGHT(m_SceneController->tilesCount().height()*POW);

    const uint AddTileOnViewport(1);//zoom < 3 ? 3 : m_SceneController->addTileOnViewport());
    int64_t leftX = (viewportRect.topLeft().x() - AddTileOnViewport*tileWidthOnView) / tileWidthOnView;
    if(leftX < 0)
        leftX = 0;
    int64_t rightX = (viewportRect.topLeft().x() + viewportRect.width() + AddTileOnViewport*tileWidthOnView) / tileWidthOnView;
    int64_t topY = (viewportRect.topLeft().y() - AddTileOnViewport*tileHeightOnView) / tileHeightOnView;
    if(topY < 0)
        topY = 0;
    int64_t bottomY = (viewportRect.topLeft().y() + viewportRect.height() + AddTileOnViewport*tileHeightOnView) / tileHeightOnView;

//    qDebug() << "leftX :" << leftX << ",rightX :" << rightX << ", topY :" << topY << ", bottomY :" << bottomY;

    if(m_zoom == zoom &&  m_leftX == leftX && m_rightX == rightX && m_topY == topY && m_bottomY == bottomY)
        return false;

//    qDebug() << "ViewController::getVisibleCells, zoom:" << zoom << ", leftX:" << leftX << ", rightX :" << rightX << ", topY :" << topY << ", bottomY :" << bottomY;

    if(m_zoom != zoom)
    {
        m_SceneController->abortLoading();
        if(m_cellMap.isEmpty() == false)
        {
//            QList<Cell> cellForDeleting;
            for(QMap<QString,Cell>::iterator it(m_cellMap.begin()); it != m_cellMap.end(); ++it)
            {
//                cellForDeleting.append(it.value());
                m_cellForDeleting.append(it.value());
            }
//            deleteCell(cellForDeleting);
//            qDebug() << "ViewPortController, m_zoom != zoom";
            m_cellMap.clear();
        }
    }


    m_leftX = leftX;
    m_rightX = rightX;
    m_topY = topY;
    m_bottomY = bottomY;
    m_lastViewRect = viewportRect;

    int64_t bottom(bottomY < MAX_HEIGHT ? bottomY : MAX_HEIGHT);
    int64_t right(rightX < MAX_WIDTH ? rightX : MAX_WIDTH);
    m_lastViewRectInTiles = QRect(leftX, topY, right-leftX, bottom-topY);
    emit signalUpdateBackgroundItem(m_lastViewRectInTiles);

    if(m_zoom == zoom)
    {
        for(int64_t y(topY); y <= bottomY && y < MAX_HEIGHT; ++y)
            for(int64_t x(leftX); x <= rightX && x < MAX_WIDTH; ++x)
            {
                Cell cell(zoom,x,y);
                cellList.append(cell);
                //qDebug() << "--->" << cell.toString();
            }
    }
    else
    {
        //! код для загрузки тайлов по спирали из центра экрана при изменении зума или апдейте
        QRect areaInTiles(leftX, topY, rightX - leftX, bottomY - topY);
        QList<QPoint> list;
        int c_x = areaInTiles.x() + areaInTiles.width()/2;
        int c_y = areaInTiles.y() + areaInTiles.height()/2;
        list.append(QPoint(c_x, c_y));
        int d = 0;
        int k_for = 0;
        //    qDebug() << "START, areaInTiles :" << areaInTiles << ", c_x :" << c_x << ", c_y :" << c_y;
        forever
        {
            d += 1;
            k_for += 2;
            //        qDebug() << "list.size :" << list.size() << ", d :" << d << ", k_for :" << k_for;

            if(c_y - d >= areaInTiles.top())
                for(int x(0); x < k_for; ++x)
                {
                    QPoint p(c_x - d + x, c_y - d);
                    if(areaInTiles.contains(p))
                    {
                        list.append(p);
                        //                    qDebug() << "111, +++" << p;
                    }
                    //                else
                    //                    qDebug() << "111, ---" << p;
                }
            if(c_x + d <= areaInTiles.right())
                for(int y(0); y < k_for; ++y)
                {
                    QPoint p(c_x + d, c_y - d + y);
                    if(areaInTiles.contains(p))
                    {
                        list.append(p);
                        //                    qDebug() << "222, +++" << p;
                    }
                    //                else
                    //                    qDebug() << "222, ---" << p;
                }
            if(c_y + d <= areaInTiles.bottom())
                for(int x(0); x < k_for; ++x)
                {
                    QPoint p(c_x + d - x, c_y + d);
                    if(areaInTiles.contains(p))
                    {
                        list.append(p);
                        //                    qDebug() << "333, +++" << p;
                    }
                    //                else
                    //                    qDebug() << "333, ---" << p;
                }
            if(c_x + d >= areaInTiles.left())
                for(int y(0); y < k_for; ++y)
                {
                    QPoint p(c_x - d, c_y + d - y);
                    if(areaInTiles.contains(p))
                    {
                        list.append(p);
                        //                    qDebug() << "444, +++" << p;
                    }
                    //                else
                    //                    qDebug() << "444, ---" << p;
                }

            if(list.size() >=  areaInTiles.width() * areaInTiles.height())
                break;
        }

        //    qDebug() << "FINISH, list.size :" << list.size() << ", area size :" << areaInTiles.width() * areaInTiles.height();
        foreach(QPoint p, list)
        {
            //        qDebug() << "X" << p.x() << ", Y" << p.y();
            Cell cell(zoom, p.x(), p.y());
            cellList.prepend(cell);
        }
    }

    m_zoom = zoom;

//    {
//        //! код для загрузки тайлов для минимапа и формирования подложки на основной сцене.
//        switch(zoom)
//        {
//        case 0:
//        case 1: zoom = 2; break;
//        case 2: zoom = 1; break;
//        case 3: zoom = 1; break;
//        case 4: zoom = 1; break;
//        default:zoom -= 4;
//        }

//        POW = (pow(2,zoom-1));
//        tileWidthOnView = (m_SceneController->tileSize().width() / POW);
//        tileHeightOnView = (m_SceneController->tileSize().height() / POW);
//        MAX_WIDTH = (m_SceneController->tilesCount().width()*POW);
//        MAX_HEIGHT = (m_SceneController->tilesCount().height()*POW);

//        leftX = viewportRect.topLeft().x() / tileWidthOnView;
//        rightX = (viewportRect.topLeft().x() + viewportRect.width()) / tileWidthOnView;
//        topY = viewportRect.topLeft().y() / tileHeightOnView;
//        bottomY = (viewportRect.topLeft().y() + viewportRect.height()) / tileHeightOnView;

//        if(leftX > 0)
//            --leftX;
//        if(rightX < MAX_WIDTH)
//            ++rightX;
//        if(topY > 0)
//            --topY;
//        if(bottomY < MAX_HEIGHT)
//            ++bottomY;

//        for(int x(leftX); x <= rightX && x < MAX_WIDTH; ++x)
//            for(int y(topY); y <= bottomY && y < MAX_HEIGHT; ++y)
//            {
//                Cell cell(zoom,x,y);
//                cell.isMainCover = false;
//                cellList.append(cell);
//            }
//    }

    return true;
}


void ViewPortController::addVisibleCellsToList(int zoom, QRectF viewportRect, QList<Cell>& cellList)
{
    double POW(pow(2,zoom-1));
    double tileWidthOnView(m_SceneController->tileSize().width() / POW);
    double tileHeightOnView(m_SceneController->tileSize().height() / POW);
    int MAX_WIDTH(m_SceneController->tilesCount().width()*POW);
    int MAX_HEIGHT(m_SceneController->tilesCount().height()*POW);

    int leftX = viewportRect.topLeft().x() / tileWidthOnView;
    int rightX = (viewportRect.topLeft().x() + viewportRect.width()) / tileWidthOnView;
    int topY = viewportRect.topLeft().y() / tileHeightOnView;
    int bottomY = (viewportRect.topLeft().y() + viewportRect.height()) / tileHeightOnView;

    for(int x(leftX); x <= rightX && x < MAX_WIDTH; ++x)
        for(int y(topY); y <= bottomY && y < MAX_HEIGHT; ++y)
        {
            Cell cell(zoom,x,y);
            cellList.append(cell);
            //qDebug() << "addVisibleCellsToList--->" << cell.toString();
        }
}


void ViewPortController::addCell(QList<Cell>& cellList)
{
//    qDebug() << "Cells for adding :";
    QList<visualize_system::URL> list;
    for(int i(0); i<cellList.count(); ++i)
    {
//        qDebug() << cellList[i].toString();
        list.append(visualize_system::URL(cellList[i].x,cellList[i].y, cellList[i].z, cellList[i].toString()));
    }

    m_SceneController->getTiles(list, m_zoom, m_viewOriginalRect , m_lastViewRectInTiles); //QRect(QPoint(m_leftX,m_topY),QPoint(m_rightX,m_bottomY)));
//    qDebug() << "end-----Cells for adding";
}

void ViewPortController::receivedImage(const QPixmap & pixmap, const QString& url)
{
    if(m_updateContent == false)
        drawImage(pixmap, url);
    else
    {
        m_ReceivedImages.append(ReceivedImage(pixmap, url));
    }
}

void ViewPortController::drawImage(const QPixmap& pixmap, const QString& url)
{
//    qDebug() << "ViewPortController::receivedImage :" << url;

    QMap<QString,Cell>::iterator it(m_cellMap.find(url));
    if(it != m_cellMap.end())
    {
        // if(pixmap.isNull() == false) //! если раскомментировать - то будут артефакты при снятии видимости и удалении у единственного провайдеар на сцене !
        {
            m_chenged = true;

            Cell cell = it.value();
            if(cell.pPixmapItem)
            {
                if(m_SceneController->isGeo())
                {
                    if(cell.isMainCover)
                    {
                        m_SceneController->getTileSceneAsinc()->removeItem(cell.pPixmapItem);
                        cell.pPixmapItem = 0;
                    }
                    else
                        delete cell.pPixmapItem;
                }
                else
                    delete cell.pPixmapItem;
            }

            const double K(pow(2,cell.z-1));
            const double tileWidthOnView(m_SceneController->tileSize().width() / K);
            const double tileHeightOnView(m_SceneController->tileSize().height() / K);

            if(cell.isMainCover)
            {
                if(m_SceneController->isGeo())
                {
                    cell.pPixmapItem = new QGraphicsPixmapItem(pixmap);
                    cell.pPixmapItem->setTransform(QTransform().scale(1/K, 1/K));
                    cell.pPixmapItem->setZValue(cell.z);
                    cell.pPixmapItem->setPos(tileWidthOnView*cell.x, tileHeightOnView*cell.y);
                    m_SceneController->getTileSceneAsinc()->addItem(cell.pPixmapItem);
                    m_cellMap.insert(url, cell);
//                    m_SceneController->getSceneInThread()->addItem(url, pixmap.toImage(), QPointF(tileWidthOnView*cell.x, tileHeightOnView*cell.y), cell.z, 1/K);
                    return; /// <-------------------
                }
                else
                {
                    cell.pPixmapItem = new QGraphicsPixmapItem(pixmap);
                    m_SceneController->getScene()->addItem(cell.pPixmapItem);
                }
            }
            else
            {
                cell.pPixmapItem = new QGraphicsPixmapItem(pixmap);
                m_SceneController->getMinimapScene()->addItem(cell.pPixmapItem);
                if(m_SceneController->viewOptimization())
                    if(/*m_SceneController->isGeo() &&*/ cell.z > 2) // создаем "фон"
                    {
                        if(cell.pBackgroundPixmapItem)
                            delete cell.pBackgroundPixmapItem;
                        cell.pBackgroundPixmapItem = new QGraphicsPixmapItem(pixmap);
                        m_SceneController->getScene()->addItem(cell.pBackgroundPixmapItem);
                        cell.pBackgroundPixmapItem->setTransform(QTransform().scale(1/K, 1/K));
                        cell.pBackgroundPixmapItem->setZValue(cell.z);
                        cell.pBackgroundPixmapItem->setPos(tileWidthOnView*cell.x, tileHeightOnView*cell.y);
                    }
            }

            cell.pPixmapItem->setTransform(QTransform().scale(1/K, 1/K));
            cell.pPixmapItem->setZValue(cell.z);
            cell.pPixmapItem->setPos(tileWidthOnView*cell.x, tileHeightOnView*cell.y);
            m_cellMap.insert(url, cell);
        }
    }
}


void ViewPortController::deleteCell(QList<Cell>& cellList)
{
    if(m_SceneController->isGeo())
    {
        QList<QGraphicsItem *> itemList;
        QStringList list;
        for(int i(0); i<cellList.count(); ++i)
        {
            if(cellList[i].isMainCover)
            {
                if(cellList[i].pPixmapItem)
                {
                    itemList.append(cellList[i].pPixmapItem);
                    cellList[i].pPixmapItem = 0;
                }
            }
            else
            {
                delete cellList[i].pPixmapItem;
                delete cellList[i].pBackgroundPixmapItem;
            }

            list.append(cellList[i].toString());
        }
        m_SceneController->removeTiles(list);

        if(itemList.isEmpty() == false)
            m_SceneController->getTileSceneAsinc()->removeItems(itemList);

//        QStringList list;
//        for(int i(0); i<cellList.count(); ++i)
//        {
//            list.append(cellList[i].toString());
//        }
//        m_SceneController->removeTiles(list);

//        if(list.isEmpty() == false)
//            m_SceneController->getSceneInThread()->removeItems(list);
    }
    else
    {
        QStringList list;
        for(int i(0); i<cellList.count(); ++i)
        {
            list.append(cellList[i].toString());
            delete cellList[i].pPixmapItem;
            delete cellList[i].pBackgroundPixmapItem;
        }
        m_SceneController->removeTiles(list);
    }

////    qDebug() << "Cells for deleting :";
//    for(int i(0); i<cellList.count(); ++i)
//    {
////        qDebug() << cellList[i].toString();
//        delete cellList[i].pPixmapItem;
//    }
////    qDebug() << "end-----Cells for deleting";
}


void ViewPortController::updateContentForSave(double Z)
{
    QList<Cell> cellsListForDeleting;
    for(QMap<QString,Cell>::iterator it(m_cellMap.begin()); it != m_cellMap.end(); ++it)
        cellsListForDeleting.append(it.value());
    if(cellsListForDeleting.isEmpty() == false)
        deleteCell(cellsListForDeleting);
    m_cellMap.clear();

    m_zoom = 0;
    m_leftX = 0;
    m_rightX = 0;
    m_topY = 0;
    m_bottomY = 0;
    localUpdate(Z, m_lastViewRect, true);
}

void ViewPortController::updateContent(bool deepRepaint)
{
    if(deepRepaint)
    {
        for(QMap<QString,Cell>::iterator it(m_cellMap.begin()); it != m_cellMap.end(); ++it)
            m_cellForDeleting.append(it.value());

        deleteCell(m_cellForDeleting);
        m_cellForDeleting.clear();
    }
    else
    {
        for(QMap<QString,Cell>::iterator it(m_cellMap.begin()); it != m_cellMap.end(); ++it)
            m_cellForDeleting.append(it.value());
    }

    m_cellMap.clear();

    const int zoom(m_zoom);
    m_zoom = 0;
    m_leftX = 0;
    m_rightX = 0;
    m_topY = 0;
    m_bottomY = 0;
    localUpdate(zoom, m_lastViewRect, true);


//////////////////////////////////////
//    if(deepRepaint)
//    {
////////////////////////////////////////
////        deleteCell(m_cellForDeleting);
////        m_cellForDeleting.clear();

//        for(QMap<QString,Cell>::iterator it(m_cellMap.begin()); it != m_cellMap.end(); ++it)
//            m_cellForDeleting.append(it.value());

////        QList<Cell> cellsListForDeleting;
////        for(QMap<QString,Cell>::iterator it(m_cellMap.begin()); it != m_cellMap.end(); ++it)
////            cellsListForDeleting.append(it.value());
////        if(cellsListForDeleting.isEmpty() == false)
////            deleteCell(cellsListForDeleting);

//        m_cellMap.clear();
////////////////////////////////////////

//        const int zoom(m_zoom);
//        m_zoom = 0;
//        m_leftX = 0;
//        m_rightX = 0;
//        m_topY = 0;
//        m_bottomY = 0;
//        localUpdate(zoom, m_lastViewRect, true);
//    }
//    else
//    {
//        emit setViewEnabled(false);
//        m_ReceivedImages.clear();
//        m_updateContent = true;

//        QList<visualize_system::URL> list;
//        for(QMap<QString,Cell>::iterator it(m_cellMap.begin()); it != m_cellMap.end(); ++it)
//        {
//            list.append(visualize_system::URL(it.value().x, it.value().y, it.value().z, it.value().toString()));
//        }
//        m_SceneController->getTiles(list, m_zoom, m_viewOriginalRect , m_lastViewRectInTiles);
//    }
}


void ViewPortController::loadingFinished()
{
    if(m_updateContent)
    {
        foreach(ReceivedImage receivedImage, m_ReceivedImages)
        {
            drawImage(receivedImage.pixmap, receivedImage.url );
        }
        m_ReceivedImages.clear();
        m_updateContent = false;
        emit setViewEnabled(true);
    }
    else
    {
//        if(m_SceneController->viewOptimization())
//            for(QMap<QString,Cell>::iterator it(m_cellMap.begin()); it != m_cellMap.end(); ++it)
//                if(it.value().pBackgroundPixmapItem)
//                    it.value().pBackgroundPixmapItem->hide();

        if(m_cellForDeleting.isEmpty() == false)
        {
//            qDebug() << "ViewPortController, loadingFinished";
            deleteCell(m_cellForDeleting);
            m_cellForDeleting.clear();
        }
    }
}




