#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QPolygon>
#include <QList>
#include <QDate>

namespace pdf_editor
{

struct OwerView
{
    bool isOriginal = false;
    bool forGallery = false;
    int scale = 1;
    QString file;
};

struct Room
{
    QString name;
    QPolygon coordsOnFloor;
    qulonglong id = 0;
};

struct RoomGroup
{
    QString name;
    QPolygon coordsOnFloor;
    QList<Room> rooms;
};

struct Floor
{
    QString name, folder;
    QList<OwerView> owerViews;
    QList<Room> rooms;
    qulonglong id = 0;
};

struct Facility
{
    QString name;
    QList<Floor> floors;
    QPolygonF coordsOnScene;
    qulonglong id = 0;

    void clear()
    {
        floors.clear();
        coordsOnScene.clear();
        name.clear();
        id = 0;
    }
};

struct Image
{
    QString name;
    double scale = 1;
    QPointF pos;
};

struct FacilityInLocation
{
    QString name, folder;
    QPolygonF coordsOnScene;
};

struct Location
{
    QString name, description;
    QPolygonF coordsOnScene;
    Image image;
    QList<FacilityInLocation> facilitiesInLocation;
};

}

#endif
