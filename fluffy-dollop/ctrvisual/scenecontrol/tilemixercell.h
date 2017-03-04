#ifndef TILEMIXERCELL_H
#define TILEMIXERCELL_H

#include<QString>
#include<QPixmap>
#include<QMap>
#include<QList>
#include<ctrrasterrender/tms/tilemapper.h>

namespace visualize_system{

class TileMixerCell
{
private:
    enum WhatSend
    {
        NO_SEND = 0,
        ONLY_FILEPREVIEW,
        FILEPREVIEW_AND_NOFILE,
        ALL
    };

    struct Cell
    {
        Cell(double _transparency, bool _clearBlackArea/*, bool _isFile*/) : transparency(_transparency), /*isFile(_isFile),*/ clearBlackArea(_clearBlackArea), tileType(TileMapper::INVALID) {}
        //bool isFile;
        QPixmap pixmap;
        double transparency;
        bool clearBlackArea;
        TileMapper::TileType tileType;
    };

public:
    TileMixerCell(QSize resultPixmapSize = QSize(256,256));
    void addLayer(uint id, double transparency, bool clearBlackArea, bool isBaseCover);
    //void addLayer(uint id, double transparency, bool clearBlackArea);
    void setLayerPixmap(uint id, const QPixmap& pixmap, TileMapper::TileType tileType);
    void clear();
    //TileMapper::TileType getPixmap(QPixmap & pixmap);
    TileMapper::TileType getResultPixmap(QPixmap & pixmap, QString text = QString());
    bool isEmpty();
    void debug();
    QList<uint> getLayersName();

private:
    QSize _resultPixmapSize;
    QMap<uint, Cell> _baseCovers, _layers;
    QList<uint> _baseCoversId , _layersId;
    //WhatSend _whatSend;
    //bool _tempPixmapSended;
    //bool _sendBaseCover;
    bool _baseCoversSended;
    uint _baseCoversCounter, _layersCounter;
};
}
#endif
