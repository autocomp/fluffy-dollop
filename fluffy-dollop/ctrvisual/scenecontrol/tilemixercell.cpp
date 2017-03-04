#include "tilemixercell.h"
#include <ctrcore/provider/dataproviderfactory.h>

#include <QPainter>
#include <QBitmap>
#include <QDebug>

#include <QTime>
#include <QFileInfo>
#include <QDir>
#include <QUrl>

using namespace visualize_system;

TileMixerCell::TileMixerCell(QSize resultPixmapSize)
    : _resultPixmapSize(resultPixmapSize)
//    , _whatSend(NO_SEND)
//    , _tempPixmapSended(false)
//    , _sendBaseCover(false)
    , _layersCounter(0)
    , _baseCoversCounter(0)
    , _baseCoversSended(false)
{
}

void TileMixerCell::addLayer(uint id, double transparency, bool clearBlackArea, bool isBaseCover)
{
    if(isBaseCover)
    {
        _baseCovers.insert(id, Cell(transparency, clearBlackArea));
        _baseCoversId.append(id);
    }
    else
    {
        _layers.insert(id, Cell(transparency, clearBlackArea));
        _layersId.append(id);
    }
}

void TileMixerCell::setLayerPixmap(uint id, const QPixmap& pixmap, TileMapper::TileType tileType)
{
    auto baseCoverIt = _baseCovers.find(id);
    if(baseCoverIt != _baseCovers.end())
    {
        baseCoverIt.value().pixmap = pixmap;
        baseCoverIt.value().tileType = tileType;
        if(tileType == TileMapper::ORIGINAL || tileType == TileMapper::INVALID)
            ++_baseCoversCounter;
        return;
    }

    auto layerIt = _layers.find(id);
    if(layerIt != _layers.end())
    {
        layerIt.value().pixmap = pixmap;
        layerIt.value().tileType = tileType;
        if(tileType == TileMapper::ORIGINAL || tileType == TileMapper::INVALID)
            ++_layersCounter;
    }
}

TileMapper::TileType TileMixerCell::getResultPixmap(QPixmap &pixmap, QString text)
{
    // qDebug() << text << "_baseCoversId.size" << _baseCoversId.size() << "_baseCoversCounter" << _baseCoversCounter << "_layersId.size" << _layersId.size() << "_layersCounter" << _layersCounter << "_baseCoversSended" << _baseCoversSended;

    if(_baseCoversCounter == _baseCoversId.size() && (_layersCounter == _layersId.size() || _baseCoversSended == false) )
    {
        pixmap = QPixmap(_resultPixmapSize);
        pixmap.fill(Qt::transparent);
        QPainter pr(&pixmap);
        int K(0);
        for(auto coverIdIt = _baseCoversId.begin(); coverIdIt != _baseCoversId.end(); ++coverIdIt)
        {
            auto coverIt = _baseCovers.find(*coverIdIt);
            if(coverIt != _baseCovers.end())
            {
                Cell cell = coverIt.value();
                if(cell.pixmap.isNull() == false)
                {
                    ++K;
                    pr.save();
                    pr.setCompositionMode(QPainter::CompositionMode_SourceOver);
                    pr.setOpacity(cell.transparency);
                    if(cell.clearBlackArea)
                    {
                        QPixmap pm(cell.pixmap);
                        pm.setMask(pm.createMaskFromColor(Qt::black));
                        pr.drawPixmap(0,0,pm);
                    }
                    else
                        pr.drawPixmap(0,0,cell.pixmap);
                    pr.restore();
                }
            }
        }
        for(auto layerIdIt = _layersId.begin(); layerIdIt != _layersId.end(); ++layerIdIt)
        {
            auto layerIt = _layers.find(*layerIdIt);
            if(layerIt != _layers.end())
            {
                Cell cell = layerIt.value();
                if(cell.pixmap.isNull() == false)
                {
                    ++K;
                    pr.save();
                    pr.setCompositionMode(QPainter::CompositionMode_SourceOver);
                    pr.setOpacity(cell.transparency);
                    if(cell.clearBlackArea)
                    {
                        QPixmap pm(cell.pixmap);
                        pm.setMask(pm.createMaskFromColor(Qt::black));
                        pr.drawPixmap(0,0,pm);
                    }
                    else
                        pr.drawPixmap(0,0,cell.pixmap);
                    pr.restore();
                }
            }
        }
        if(text.isEmpty() == false)
        {
            text.append(QString::fromUtf8(", всего : ") + QString::number(K));
            QRect R(0,0,_resultPixmapSize.width()-1,_resultPixmapSize.height()-1);
            pr.save();
            pr.setPen(QPen(Qt::red));
            pr.drawRect(R);
            pr.drawText(R, Qt::AlignCenter, text);
            pr.restore();
        }

        if(_layersCounter == _layersId.size())
        {
            //qDebug() << text << "ORIGINAL";
            return TileMapper::ORIGINAL;
        }

        if(_baseCoversSended == false)
        {
            //qDebug() << text << "SCALED";
            _baseCoversSended = true;
            return TileMapper::SCALED;
        }
    }

    //qDebug() << text << "INVALID";
    return TileMapper::INVALID;
}


///---------------------------------------------------------

/*
void TileMixerCell::addLayer(uint id, double transparency, bool clearBlackArea)
{
    QSharedPointer<data_system::AbstractDataProvider> dp = data_system::DataProviderFactory::instance()->getProvider(id);
    QUrl url(dp->getProviderUrl());
    QFileInfo fi(url.toLocalFile());
    bool isFile(fi.exists());
    _layers.insert(id, Cell(transparency, clearBlackArea, isFile));
    _layersName.prepend(id);
}

void TileMixerCell::setLayerPixmap(uint id, const QPixmap& pixmap, TileMapper::TileType tileType)
{
    auto layerIt = _layers.find(id);
    if(layerIt != _layers.end())
    {
        ++_layersCounter;
        layerIt.value().pixmap = pixmap;
        layerIt.value().tileType = tileType;
        if(layerIt.value().isFile == false)
            _sendBaseCover = true;
    }
}

void TileMixerCell::clear()
{
    _layers.clear();
    _layersName.clear();
}

// картинка возвращается всегда, склеивается из того что есть.
TileMapper::TileType TileMixerCell::getPixmap(QPixmap &pixmap)
{
    bool allDataLoaded = true;
    for(auto it = _layers.begin(); it != _layers.end(); ++it)
        switch(it.value().tileType){
        case TileMapper::INVALID :
            allDataLoaded = false;
            break;
        case TileMapper::SCALED :
            allDataLoaded = false;
            break;
        }

    pixmap = QPixmap(_resultPixmapSize);
    pixmap.fill(Qt::transparent);
    QPainter pr(&pixmap);
    for(auto nameIt = _layersName.begin(); nameIt != _layersName.end(); ++nameIt)
    {
        auto layerIt = _layers.find(*nameIt);
        if(layerIt != _layers.end())
        {
            Cell cell = layerIt.value();
            if(cell.pixmap.isNull() == false)
            {
                pr.save();
                pr.setCompositionMode(QPainter::CompositionMode_SourceOver);
                pr.setOpacity(cell.transparency);
                if(cell.clearBlackArea)
                {
                    QPixmap pm(cell.pixmap);
                    pm.setMask(pm.createMaskFromColor(Qt::black));
                    pr.drawPixmap(0,0,pm);
                }
                else
                    pr.drawPixmap(0,0,cell.pixmap);
                pr.restore();
            }
        }
    }
    return allDataLoaded ? TileMapper::ORIGINAL : TileMapper::SCALED;
}

// картинка возвращается только если есть все предварительные или результирующие данные.
TileMapper::TileType TileMixerCell::getResultPixmap(QPixmap &pixmap, QString text)
{
    TileMapper::TileType tileType(TileMapper::ORIGINAL);
    for(auto it = _layers.begin(); it != _layers.end(); ++it)
    {
        switch(it.value().tileType){
        case TileMapper::INVALID :
            if(_sendBaseCover)
                break;
            else
                return TileMapper::INVALID;
        case TileMapper::ORIGINAL :
            break;
        case TileMapper::SCALED :
            tileType = TileMapper::SCALED;
            break;
        }
    }

    if(_sendBaseCover == false)
    {
        if(tileType == TileMapper::SCALED)
        {
            if(_tempPixmapSended)
                return TileMapper::INVALID;
            else
            {
//                text = QString::fromUtf8("SCALED");
                _tempPixmapSended = true;
            }
        }
        else
        {
//            text = QString::fromUtf8("ORIGINAL");
        }
    }
    else
    {
        if(tileType == TileMapper::INVALID)
        {
//            text = QString::fromUtf8("_sendBaseCover + INVALID");
            tileType = TileMapper::SCALED;
        }
        else
        {
            if(tileType == TileMapper::SCALED)
            {
//                text = QString::fromUtf8("_sendBaseCover + SCALED");
            }
            else
            {
                if(_layersCounter < _layers.size())
                {
//                    text = QString::fromUtf8("_sendBaseCover + SCALED, ") + QString::number(_layersCounter) + QString::fromUtf8(" из ") + QString::number(_layers.size());
                    tileType = TileMapper::SCALED;
                }
                else
                {
//                    text = QString::fromUtf8("_sendBaseCover + ORIGINAL, ") + QString::number(_layersCounter) + QString::fromUtf8(" из ") + QString::number(_layers.size());
                }
            }
        }
        _sendBaseCover = false;
    }

    pixmap = QPixmap(_resultPixmapSize);
    pixmap.fill(Qt::transparent);
    QPainter pr(&pixmap);
    int K(0);
    for(auto nameIt = _layersName.begin(); nameIt != _layersName.end(); ++nameIt)
    {
        auto layerIt = _layers.find(*nameIt);
        if(layerIt != _layers.end())
        {
            Cell cell = layerIt.value();
            if(cell.pixmap.isNull() == false)
            {
                ++K;
                pr.save();
                pr.setCompositionMode(QPainter::CompositionMode_SourceOver);
                pr.setOpacity(cell.transparency);
                if(cell.clearBlackArea)
                {
                    QPixmap pm(cell.pixmap);
                    pm.setMask(pm.createMaskFromColor(Qt::black));
                    pr.drawPixmap(0,0,pm);
                }
                else
                    pr.drawPixmap(0,0,cell.pixmap);
                pr.restore();
            }
        }
    }
    if(text.isEmpty() == false)
    {
        QRect R(0,0,_resultPixmapSize.width()-1,_resultPixmapSize.height()-1);
        pr.save();
        pr.setPen(QPen(Qt::red));
        pr.drawRect(R);
        pr.drawText(R, Qt::AlignCenter, text);
        pr.restore();
    }

    return tileType;
}
*/








//TileMapper::TileType TileMixerCell::getResultPixmap(QPixmap &pixmap, const QString & text)
//{
////    bool allDataLoaded = true;
////    for(auto it = _layers.begin(); it != _layers.end(); ++it)
////        switch(it.value().tileType){
////        case TileMapper::INVALID :
////            allDataLoaded = false;
////            break;
////        case TileMapper::SCALED :
////            allDataLoaded = false;
////            break;
////        }

////////
//    TileMapper::TileType tileType(TileMapper::ORIGINAL);
//    for(auto it = _layers.begin(); it != _layers.end(); ++it)
//    {
//        switch(it.value().tileType){
//        case TileMapper::INVALID :
//            return TileMapper::INVALID;
//        case TileMapper::ORIGINAL :
//            break;
//        case TileMapper::SCALED :
//            tileType = TileMapper::SCALED;
//            break;
//        }
//    }

//    if(tileType == TileMapper::SCALED)
//    {
//        if(_tempPixmapSended)
//            return TileMapper::INVALID;
//        else
//            _tempPixmapSended = true;
//    }
////////

//    pixmap = QPixmap(_resultPixmapSize);
//    pixmap.fill(Qt::transparent);
//    QPainter pr(&pixmap);
////    int count(0);
////    QString _text(text + "  ");
//    for(auto nameIt = _layersName.begin(); nameIt != _layersName.end(); ++nameIt)
//    {
//        auto layerIt = _layers.find(*nameIt);
//        if(layerIt != _layers.end())
//        {
////            ++count;
//            Cell cell = layerIt.value();
////            QString fileName;
////            if(text.isEmpty() == false)
////            {
//////                qDebug() << "--------------";
////                if(cell.isFile)
////                {
////                    QString fileName;
////                    QUrl url(layerIt.key());
////                    QFileInfo fi(url.toLocalFile());
////                    fileName.append(fi.fileName());
//////                    qDebug() << layerIt.key() << "fileName" << fileName;
////                }
////                else
////                    fileName.append("tms");
////            }

////            QString T;
//            if(cell.pixmap.isNull() == false)
//            {
//                pr.save();
//                pr.setCompositionMode(QPainter::CompositionMode_SourceOver);
//                pr.setOpacity(cell.transparency);
//                if(cell.clearBlackArea)
//                {
//                    QPixmap pm(cell.pixmap);
//                    pm.setMask(pm.createMaskFromColor(Qt::black));
//                    pr.drawPixmap(0,0,pm);
//                }
//                else
//                    pr.drawPixmap(0,0,cell.pixmap);
//                pr.restore();

////                switch(cell.tileType)
////                {
////                case TileMapper::INVALID :
////                    T = QString('  ' + QString::number(count) + "_" + fileName + "_INV");
////                    break;
////                case TileMapper::ORIGINAL :
////                    T = QString('  ' + QString::number(count) + "_" + fileName + "_ORIG");
////                    break;
////                case TileMapper::SCALED :
////                    T = QString('  ' + QString::number(count) + "_" + fileName + "_SCL");
////                    break;
////                }
//            }
////            _text.append(T);
//        }
//    }
////    if(text.isEmpty() == false)
////    {
////        QRect R(0,0,_resultPixmapSize.width()-1,_resultPixmapSize.height()-1);
////        pr.save();
////        pr.setPen(QPen(Qt::blue));
////        pr.drawRect(R);
////        pr.drawText(R, _text);
////        pr.restore();
////    }


//    return tileType;
////    return allDataLoaded ? TileMapper::ORIGINAL : TileMapper::SCALED;
//}


//TileMapper::TileType TileMixerCell::getResultPixmap(QPixmap &pixmap, const QString & text)
//{
//// Принцип такой :
////  Сначала нужен анализ, что есть из трёх вариантов : только НЕ файлы, только файлы, и те и другие.
////  Предварительная отправка :
////      когда сформированы все предварительные файлы // ONLY_FILEPREVIEW
////      когда сформированы все НЕ файлы и все предварительные файлы // FILEPREVIEW_AND_NOFILE
////  Окончательная отправка :
////      когда есть все на беловую // ALL
////      когда сформированы все НЕ файлы, а файлов нет // ALL

////    if(_layers.isEmpty())
////        return TileMapper::INVALID;

////    // считаем сколько всего файлов и не файлов, и сколько получено вообще, сколько получено из файлов превьюх.
////    uint totalNoFile(0), totalNoFileLoaded(0), totalFile(0), totalFileScaledLoaded(0), totalFileOriginalLoaded(0);
////    for(auto it = _layers.begin(); it != _layers.end(); ++it)
////    {
////        Cell cell = it.value();
////        if(cell.isFile)
////            totalFile += 1;
////        else
////            totalNoFile += 1;

////        switch(cell.tileType){
////        case TileMapper::INVALID :
////            if(cell.isFile)
////                return TileMapper::INVALID;
////            break;
////        case TileMapper::ORIGINAL :
////            if(cell.isFile)
////                totalFileOriginalLoaded += 1;
////            else
////                totalNoFileLoaded += 1;
////            break;
////        case TileMapper::SCALED :
////            totalFileScaledLoaded += 1;
////            break;
////        }
////    }

////    WhatSend whatSend = _whatSend;
////    TileMapper::TileType tileType(TileMapper::INVALID);
////    if( (totalFile + totalNoFile) == (totalFileOriginalLoaded + totalNoFileLoaded) )
////    {
////        whatSend = ALL;
////        tileType = TileMapper::ORIGINAL;
//////        qDebug() << "ALL, ORIGINAL" << text << ", whatSend:" << whatSend << ", _whatSend:" << _whatSend << ", tileType:" << tileType << ", totalNoFile:" << totalNoFile << ", totalNoFileLoaded:" << totalNoFileLoaded << ", totalFile:" << totalFile << ", totalFileScaledLoaded:" << totalFileScaledLoaded << ", totalFileOriginalLoaded:" << totalFileOriginalLoaded;
////    }
////    else if(totalFile == (totalFileScaledLoaded + totalFileOriginalLoaded) )
////    {
////        if(totalNoFile == totalNoFileLoaded)
////        {
////            whatSend = FILEPREVIEW_AND_NOFILE;
////            tileType = TileMapper::SCALED;
//////            qDebug() << "FILEPREVIEW_AND_NOFILE, SCALED" << text << ", whatSend:" << whatSend << ", _whatSend:" << _whatSend << ", tileType:" << tileType << ", totalNoFile:" << totalNoFile << ", totalNoFileLoaded:" << totalNoFileLoaded << ", totalFile:" << totalFile << ", totalFileScaledLoaded:" << totalFileScaledLoaded << ", totalFileOriginalLoaded:" << totalFileOriginalLoaded;
////        }
////        else
////        {
////            whatSend = ONLY_FILEPREVIEW;
////            tileType = TileMapper::SCALED;
//////            qDebug() << "ONLY_FILEPREVIEW, SCALED" << text << ", whatSend:" << whatSend << ", _whatSend:" << _whatSend << ", tileType:" << tileType << ", totalNoFile:" << totalNoFile << ", totalNoFileLoaded:" << totalNoFileLoaded << ", totalFile:" << totalFile << ", totalFileScaledLoaded:" << totalFileScaledLoaded << ", totalFileOriginalLoaded:" << totalFileOriginalLoaded;
////        }
////    }
//////    else
//////    {
//////        qDebug() << "else :" << text;
//////    }

////    if(tileType == TileMapper::INVALID)
////        return TileMapper::INVALID; // мы не зашли ни в один блок !

////    if(_whatSend == whatSend)
////        return TileMapper::INVALID; // мы уже делали подобную отправку !

////    _whatSend = whatSend; // состояние отправки изменилось !

//////    switch (_whatSend) {
//////    case NO_SEND:
//////        qDebug() << "NO_SEND:" << text;
//////        break;
//////    case ONLY_FILEPREVIEW:
//////        qDebug() << "ONLY_FILEPREVIEW:" << text;
//////        break;
//////    case FILEPREVIEW_AND_NOFILE:
//////        qDebug() << "FILEPREVIEW_AND_NOFILE:" << text;
//////        break;
//////    case ALL:
//////        qDebug() << "ALL:" << text;
//////        break;
//////    default:
//////        break;
//////    }


////    pixmap = QPixmap(_resultPixmapSize);
////    pixmap.fill(Qt::transparent);
////    QPainter pr(&pixmap);
////    int count(0);
////    QString _text(text + "  ");
////    for(auto nameIt = _layersName.begin(); nameIt != _layersName.end(); ++nameIt)
////    {
////        auto layerIt = _layers.find(*nameIt);
////        if(layerIt != _layers.end())
////        {
////            ++count;
////            Cell cell = layerIt.value();
////            QString fileName;
////            if(text.isEmpty() == false)
////            {
////                if(cell.isFile)
////                {
////                    QUrl url(layerIt.key());
////                    QFileInfo fi(url.toLocalFile());
////                    fileName.append(fi.fileName());
////                }
////                else
////                    fileName.append("tms");
////            }

////            QString T;
////            if(cell.pixmap.isNull() == false)
////            {
////                pr.save();
////                pr.setCompositionMode(QPainter::CompositionMode_SourceOver);
////                pr.setOpacity(cell.transparency);
////                pr.drawPixmap(0,0,cell.pixmap);
////                pr.restore();
////                switch(cell.tileType){
////                case TileMapper::INVALID :
////                    T = QString('  ' + QString::number(count) + "_" + fileName + "_INV");
////                    break;
////                case TileMapper::ORIGINAL :
////                    T = QString('  ' + QString::number(count) + "_" + fileName + "_ORIG");
////                    break;
////                case TileMapper::SCALED :
////                    T = QString('  ' + QString::number(count) + "_" + fileName + "_SCL");
////                    break;
////                }
////            }
////            else
////            {
////                T = QString('  ' + QString::number(count) + "_" + fileName + "_NULL");
////            }

////            _text.append(T);

////        }
////    }
////    if(text.isEmpty() == false)
////    {
////        QRect R(0,0,_resultPixmapSize.width()-1,_resultPixmapSize.height()-1);
////        pr.save();
////        pr.setPen(QPen(Qt::blue));
////        pr.drawRect(R);
////        pr.drawText(R, _text);
////        pr.restore();
////    }

////    return tileType;

/////-------------------------------------------

//    TileMapper::TileType tileType(TileMapper::ORIGINAL);
//    for(auto it = _layers.begin(); it != _layers.end(); ++it)
//    {
//        switch(it.value().tileType){
//        case TileMapper::INVALID :
//            return TileMapper::INVALID;
//        case TileMapper::ORIGINAL :
//            break;
//        case TileMapper::SCALED :
//            tileType = TileMapper::SCALED;
//            break;
//        }
//    }

//    if(tileType == TileMapper::SCALED)
//    {
//        if(_tempPixmapSended)
//            return TileMapper::INVALID;
//        else
//            _tempPixmapSended = true;
//    }

//    pixmap = QPixmap(_resultPixmapSize);
//    pixmap.fill(Qt::transparent);
//    QPainter pr(&pixmap);
//    int count(0);
////    QString _text(text + "  ");
//    for(auto nameIt = _layersName.begin(); nameIt != _layersName.end(); ++nameIt)
//    {
//        auto layerIt = _layers.find(*nameIt);
//        if(layerIt != _layers.end())
//        {
//            ++count;
//            Cell cell = layerIt.value();
////            QString fileName;
////            if(text.isEmpty() == false)
////            {
////                qDebug() << "--------------";
////                if(cell.isFile)
////                {
////                    QString fileName;
////                    QUrl url(layerIt.key());
////                    QFileInfo fi(url.toLocalFile());
////                    fileName.append(fi.fileName());
////                    qDebug() << layerIt.key() << "fileName" << fileName;
////                }
////                else
////                    fileName.append("tms");
////            }

//            //QString T;
//            if(cell.pixmap.isNull() == false)
//            {
//                pr.save();
//                pr.setCompositionMode(QPainter::CompositionMode_SourceOver);
//                pr.setOpacity(cell.transparency);
//                pr.drawPixmap(0,0,cell.pixmap);
//                pr.restore();

////                switch(cell.tileType)
////                {
////                case TileMapper::INVALID :
////                    T = QString('  ' + QString::number(count) + "_" + fileName + "_INV");
////                    break;
////                case TileMapper::ORIGINAL :
////                    T = QString('  ' + QString::number(count) + "_" + fileName + "_ORIG");
////                    break;
////                case TileMapper::SCALED :
////                    T = QString('  ' + QString::number(count) + "_" + fileName + "_SCL");
////                    break;
////                }
//            }
//            //_text.append(T);
//        }
//    }
//    if(text.isEmpty() == false)
//    {
//        QRect R(0,0,_resultPixmapSize.width()-1,_resultPixmapSize.height()-1);
//        pr.save();
//        pr.setPen(QPen(Qt::blue));
//        pr.drawRect(R);
//        pr.drawText(R, text);
//        pr.restore();
//    }

//    return tileType;
//}

bool TileMixerCell::isEmpty()
{
    return _baseCoversId.isEmpty() && _layersId.isEmpty();
}

QList<uint> TileMixerCell::getLayersName()
{
    QList<uint> ids;
    ids.append(_baseCoversId);
    ids.append(_layersId);
    return ids;
}

void TileMixerCell::debug()
{
//    int N(0);
//    for(auto it = _layers.begin(); it != _layers.end(); ++it)
//    {
//        switch(it.value().tileType)
//        {
//        case TileMapper::INVALID :
//            qDebug() << ++N << "INVALID -" << it.key();
//            break;
//        case TileMapper::ORIGINAL :
//            qDebug() << ++N << "ORIGINAL -" << it.key();
//            break;
//        case TileMapper::SCALED :
//            qDebug() << ++N << "SCALED -" << it.key();
//            break;
//        }
//    }
//    qDebug() << "-------------------";
}



















