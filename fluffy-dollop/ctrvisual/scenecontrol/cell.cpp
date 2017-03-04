#include <QDebug>
#include <QObject>
#include <QString>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

#include "cell.h"

using namespace visualize_system;

Cell::Cell(int _z, int64_t _x, int64_t _y)
    : z(_z), x(_x), y(_y), pPixmapItem(0), pBackgroundPixmapItem(0), isMainCover(true){}

Cell::Cell(const Cell& other)
    : z(other.z), x(other.x), y(other.y), pPixmapItem(other.pPixmapItem), pBackgroundPixmapItem(other.pBackgroundPixmapItem), isMainCover(other.isMainCover) {}

Cell& Cell::operator = (const Cell& other)
{
    z = other.z;
    x = other.x;
    y = other.y;
    pPixmapItem = other.pPixmapItem;
    pBackgroundPixmapItem = other.pBackgroundPixmapItem;
    isMainCover = other.isMainCover;
    return *this;
}

bool Cell::operator == (const Cell& other)
{
    return z == other.z && x == other.x && y == other.y;
}

QString Cell::toString() const
{
    return QString::number(z) + " " + QString::number(x) + " " + QString::number(y);
}

bool Cell::fromString(const QString& str, Cell& cell)
{
    const QRegExp reg(" ");
    uint tagCount = 0;
    forever
    {
        QString tag = str.section(reg, tagCount, tagCount);
        if(tag.isEmpty())
            return tagCount == 3 ? true : false;
        else
            switch (tagCount)
            {
            case 0: cell.z = tag.toInt(); break;
            case 1: cell.x = tag.toInt(); break;
            case 2: cell.y = tag.toInt(); break;
            default: return false;
            }
        ++tagCount;
    }
    return false;
}
























