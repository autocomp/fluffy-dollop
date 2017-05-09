#ifndef AREA_TREEWIDGET_ITEMS_H
#define AREA_TREEWIDGET_ITEMS_H
#include <QTreeWidgetItem>

class FloorTreeWidgetItem: public QTreeWidgetItem
{
public:
    FloorTreeWidgetItem( QTreeWidgetItem* parent ):
        QTreeWidgetItem( parent )
    {}

    bool operator < ( const QTreeWidgetItem& other ) const;
};

#endif // AREA_TREEWIDGET_ITEMS_H
