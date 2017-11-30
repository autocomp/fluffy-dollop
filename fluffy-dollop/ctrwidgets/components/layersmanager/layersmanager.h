#ifndef LAYERSMANAGER_H
#define LAYERSMANAGER_H

#include <ctrcore/plugin/embifacenotifier.h>
#include <regionbiz/rb_manager.h>
#include <QObject>

class QTreeWidget;
class QPushButton;

namespace layers_manager_form {
class LayersManagerForm;
}

class LayersManager : public QObject
{
    Q_OBJECT
public:
    LayersManager();
    ~LayersManager();


private slots:
    void setVisible(bool on_off);
    void slotCloseWindow();
    void slotObjectSelectionChanged(uint64_t,uint64_t);
    void init();
    void syncButtonChecked();

private:
    void reset();
    void reload(regionbiz::BaseAreaPtr ptr, bool isGeoScene);

    QPushButton* _layersButton;
    EmbIFaceNotifier* _iface = nullptr;
    uint64_t _currentObject = 0;
    layers_manager_form::LayersManagerForm * _layersManagerForm = nullptr;

};


#endif
