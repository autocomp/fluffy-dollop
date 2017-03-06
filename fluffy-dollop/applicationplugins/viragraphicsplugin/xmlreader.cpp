#include "xmlreader.h"

#include <QDomDocument>
#include <QFile>
#include <QString>
#include <QList>
#include <QPolygon>
#include <QDebug>

bool pdf_editor::XmlReader::readFacility(const QString &xmlFile, Facility& facility)
{
    QDomDocument doc;
    QFile file(xmlFile);
    bool ok;
    if(file.open(QIODevice::ReadOnly))
        ok = doc.setContent(file.readAll());
    file.close();

    if( ! ok)
        return false;

    QDomElement facilityElem = doc.documentElement();
    facility.name = facilityElem.attribute("name");
    QDomNode facilityChild = facilityElem.firstChild();
    while(!facilityChild.isNull())
    {
        QDomElement facilityChildEl = facilityChild.toElement();
        if(!facilityChildEl.isNull()) // Floor
        {
            if(facilityChildEl.tagName() == QString("Floor"))
            {
                Floor floor;
                floor.name = facilityChildEl.attribute("name");
                floor.folder = facilityChildEl.attribute("folder");
                QDomNode floorChild = facilityChildEl.firstChild();
                while(!floorChild.isNull())
                {
                    QDomElement floorChildEl = floorChild.toElement();
                    if(!floorChildEl.isNull())
                    {
                        if(floorChildEl.tagName() == QString("OwerViews"))
                        {
                            QDomNode owerViewsChild = floorChildEl.firstChild();
                            while(!owerViewsChild.isNull())
                            {
                                QDomElement owerViewsChildEl = owerViewsChild.toElement();
                                OwerView owerView;
                                owerView.scale = owerViewsChildEl.attribute("scale").toInt();
                                owerView.file = owerViewsChildEl.attribute("file");
                                if(owerViewsChildEl.attribute("original").isEmpty() == false)
                                    owerView.isOriginal = true;
                                if(owerViewsChildEl.attribute("for_gallery").isEmpty() == false)
                                    owerView.forGallery = true;
                                floor.owerViews.append(owerView);
                                owerViewsChild = owerViewsChild.nextSibling();
                            }
                        }
                        else if(floorChildEl.tagName() == QString("Room"))
                        {
                            Room room;
                            room.name = floorChildEl.attribute("name");
                            QDomNode coordinatsChild = floorChildEl.firstChild();
                            QDomElement coordinatsChildEl = coordinatsChild.toElement();
                            if(!coordinatsChild.isNull() && coordinatsChildEl.tagName() == QString("Coordinats"))
                            {
                                QDomNode coordinatChild = coordinatsChildEl.firstChild();
                                while(!coordinatChild.isNull())
                                {
                                    QDomElement coordinatChildEl = coordinatChild.toElement();
                                    QPoint p(coordinatChildEl.attribute("x").toInt(), coordinatChildEl.attribute("y").toInt());
                                    room.coordsOnFloor.append(p);
                                    coordinatChild = coordinatChild.nextSibling();
                                }
                            }
                            floor.rooms.append(room);
                        }
                    }
                    floorChild = floorChild.nextSibling();
                }
                facility.floors.append(floor);
            }
        }
        facilityChild = facilityChild.nextSibling();
    }
    return true;
}

bool pdf_editor::XmlReader::readLocation(const QString& xmlFile, Location& location)
{
    bool ok;
    QDomDocument doc;
    QFile file(xmlFile);
    if(file.open(QIODevice::ReadOnly))
        ok = doc.setContent(file.readAll());
    file.close();

    if( ! ok)
        return false;

    QDomElement locationElem = doc.documentElement();
    location.name = locationElem.attribute("name");
    QDomNode locationChild = locationElem.firstChild();
    while(!locationChild.isNull())
    {
        QDomElement locationChildEl = locationChild.toElement();
        if(locationChildEl.tagName() == QString("Description"))
        {
            location.description = locationChildEl.attribute("text");
        }
        else if(locationChildEl.tagName() == QString("Image"))
        {
            location.image.name = locationChildEl.attribute("file");
            location.image.scale = locationChildEl.attribute("scale").toDouble();
            location.image.pos = QPointF(locationChildEl.attribute("x").toDouble(),
                                         locationChildEl.attribute("y").toDouble());
        }
        else if(locationChildEl.tagName() == QString("Coordinats"))
        {
            QDomNode coordinatChild = locationChildEl.firstChild();
            while(!coordinatChild.isNull())
            {
                QDomElement coordinatChildEl = coordinatChild.toElement();
                QPointF p(coordinatChildEl.attribute("x").toDouble(), coordinatChildEl.attribute("y").toDouble());
                location.coordsOnScene.append(p);
                coordinatChild = coordinatChild.nextSibling();
            }
        }
        else if(locationChildEl.tagName() == QString("Facilities"))
        {
            QDomNode facilityChild = locationChildEl.firstChild();
            while(!facilityChild.isNull())
            {
                QDomElement facilityChildEl = facilityChild.toElement();
                FacilityInLocation facility;
                facility.name = facilityChildEl.attribute("name");
                facility.folder = facilityChildEl.attribute("folder");

                QDomNode coordsNode = facilityChildEl.firstChild();
                if(!coordsNode.isNull() && coordsNode.toElement().tagName() == QString("Coordinats"))
                {
                    QDomNode coordinatChild = coordsNode.toElement().firstChild();
                    while(!coordinatChild.isNull())
                    {
                        QDomElement coordinatChildEl = coordinatChild.toElement();
                        QPointF p(coordinatChildEl.attribute("x").toDouble(), coordinatChildEl.attribute("y").toDouble());
                        facility.coordsOnScene.append(p);
                        coordinatChild = coordinatChild.nextSibling();
                    }
                }

                location.facilitiesInLocation.append(facility);
                facilityChild = facilityChild.nextSibling();
            }
        }
        locationChild = locationChild.nextSibling();
    }
    return true;
}





