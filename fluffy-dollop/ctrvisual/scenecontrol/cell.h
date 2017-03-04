#ifndef CELL_H
#define CELL_H

#include <QString>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

namespace visualize_system {

class Cell
{
public:
    int z;
    int64_t x, y;
    QGraphicsPixmapItem* pPixmapItem;
    QGraphicsPixmapItem* pBackgroundPixmapItem;
    bool isMainCover;

    Cell(int _z, int64_t _x, int64_t _y);
    Cell(const Cell& other);
    Cell& operator = (const Cell& other);
    bool operator == (const Cell& other);
    QString toString() const;
    static bool fromString(const QString &str, Cell &cell);
};

}

#endif
