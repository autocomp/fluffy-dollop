#ifndef LAYERSMENU_H
#define LAYERSMENU_H

#include <QLabel>

class LayersMenu : public QLabel
{
    Q_OBJECT
public:
    LayersMenu();
signals:
    void getNeedVisibleRasters(bool & axis, bool & sizes, bool & waterDisposal, bool & waterSupply, bool & heating, bool & electricity, bool & doors);
    void rastersVisibleChanged();
protected:
    virtual void mousePressEvent(QMouseEvent* e);
};

#endif // LAYERSMENU_H

