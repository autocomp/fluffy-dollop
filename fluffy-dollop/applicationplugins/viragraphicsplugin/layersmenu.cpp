#include "layersmenu.h"
#include <ctrcore/ctrcore/ctrconfig.h>
#include <QMenu>
#include <QAction>

LayersMenu::LayersMenu()
    : QLabel(QString::fromUtf8("Слои"))
{
}

void LayersMenu::mousePressEvent(QMouseEvent *)
{
    QMenu menu(this);

    bool axis, sizes, waterDisposal, waterSupply, heating, electricity, doors;
    emit getNeedVisibleRasters(axis, sizes, waterDisposal, waterSupply, heating, electricity, doors);

    QAction * allAct = menu.addAction(QString::fromUtf8("все слои"));
    allAct->setCheckable(true);
    allAct->setChecked(true);
    QAction * separator = menu.addSeparator();
    bool removeAllAct(true);

    QAction * axisAct = nullptr;
    if(axis)
    {
        removeAllAct = false;
        axisAct = menu.addAction(QString::fromUtf8("оси"));
        axisAct->setCheckable(true);
        QVariant axisRasterVisible = CtrConfig::getValueByName("application_settings.axisRasterVisible", true, true);
        axisAct->setChecked(axisRasterVisible.toBool());
        if(axisRasterVisible.toBool() == false)
            allAct->setChecked(false);
    }
    QAction * sizesAct = nullptr;
    if(sizes)
    {
        removeAllAct = false;
        sizesAct = menu.addAction(QString::fromUtf8("размер"));
        sizesAct->setCheckable(true);
        QVariant sizesRasterVisible = CtrConfig::getValueByName("application_settings.sizesRasterVisible", true, true);
        sizesAct->setChecked(sizesRasterVisible.toBool());
        if(sizesRasterVisible.toBool() == false)
            allAct->setChecked(false);
    }
    QAction * waterDisposalAct = nullptr;
    if(waterDisposal)
    {
        removeAllAct = false;
        waterDisposalAct = menu.addAction(QString::fromUtf8("водоотведение"));
        waterDisposalAct->setCheckable(true);
        QVariant waterDisposalRasterVisible = CtrConfig::getValueByName("application_settings.waterDisposalRasterVisible", true, true);
        waterDisposalAct->setChecked(waterDisposalRasterVisible.toBool());
        if(waterDisposalRasterVisible.toBool() == false)
            allAct->setChecked(false);
    }
    QAction * waterSupplyAct = nullptr;
    if(waterSupply)
    {
        removeAllAct = false;
        waterSupplyAct = menu.addAction(QString::fromUtf8("водоснабжение"));
        waterSupplyAct->setCheckable(true);
        QVariant waterSupplyRasterVisible = CtrConfig::getValueByName("application_settings.waterSupplyRasterVisible", true, true);
        waterSupplyAct->setChecked(waterSupplyRasterVisible.toBool());
        if(waterSupplyRasterVisible.toBool() == false)
            allAct->setChecked(false);
    }
    QAction * heatingAct = nullptr;
    if(heating)
    {
        removeAllAct = false;
        heatingAct = menu.addAction(QString::fromUtf8("отопление"));
        heatingAct->setCheckable(true);
        QVariant heatingRasterVisible = CtrConfig::getValueByName("application_settings.heatingRasterVisible", true, true);
        heatingAct->setChecked(heatingRasterVisible.toBool());
        if(heatingRasterVisible.toBool() == false)
            allAct->setChecked(false);
    }
    QAction * electricityAct = nullptr;
    if(electricity)
    {
        removeAllAct = false;
        electricityAct = menu.addAction(QString::fromUtf8("электрика"));
        electricityAct->setCheckable(true);
        QVariant electricityRasterVisible = CtrConfig::getValueByName("application_settings.electricityRasterVisible", true, true);
        electricityAct->setChecked(electricityRasterVisible.toBool());
        if(electricityRasterVisible.toBool() == false)
            allAct->setChecked(false);
    }
    QAction * doorsAct = nullptr;
    if(doors)
    {
        removeAllAct = false;
        doorsAct = menu.addAction(QString::fromUtf8("двери"));
        doorsAct->setCheckable(true);
        QVariant doorsRasterVisible = CtrConfig::getValueByName("application_settings.doorsRasterVisible", true, true);
        doorsAct->setChecked(doorsRasterVisible.toBool());
        if(doorsRasterVisible.toBool() == false)
            allAct->setChecked(false);
    }

    if(removeAllAct)
    {
        menu.removeAction(allAct);
        allAct = nullptr;

        menu.removeAction(separator);
        separator = nullptr;
    }

    QAction * defectsAct = menu.addAction(QString::fromUtf8("дефекты"));
    defectsAct->setCheckable(true);
    QVariant defectsRasterVisible = CtrConfig::getValueByName("application_settings.defectsRasterVisible", true, true);
    defectsAct->setChecked(defectsRasterVisible.toBool());

    if(allAct)
    {
        if(defectsRasterVisible.toBool() == false)
            allAct->setChecked(false);
        if(allAct->isChecked())
            allAct->setDisabled(true);
    }

    QPoint globalPos = mapToGlobal(pos());
    QAction * act = menu.exec(QPoint(globalPos.x(), globalPos.y() + 22));
    if( ! act )
        return;

    if(act == allAct)
    {
        if(axis)
            CtrConfig::setValueByName("application_settings.axisRasterVisible", true);
        if(sizes)
            CtrConfig::setValueByName("application_settings.sizesRasterVisible", true);
        if(waterDisposal)
            CtrConfig::setValueByName("application_settings.waterDisposalRasterVisible", true);
        if(waterSupply)
            CtrConfig::setValueByName("application_settings.waterSupplyRasterVisible", true);
        if(heating)
            CtrConfig::setValueByName("application_settings.heatingRasterVisible", true);
        if(electricity)
            CtrConfig::setValueByName("application_settings.electricityRasterVisible", true);
        if(doors)
            CtrConfig::setValueByName("application_settings.doorsRasterVisible", true);
        CtrConfig::setValueByName("application_settings.defectsRasterVisible", true);
    }
    else if(act == axisAct)
        CtrConfig::setValueByName("application_settings.axisRasterVisible", act->isChecked());
    else if(act == sizesAct)
        CtrConfig::setValueByName("application_settings.sizesRasterVisible", act->isChecked());
    else if(act == waterDisposalAct)
        CtrConfig::setValueByName("application_settings.waterDisposalRasterVisible", act->isChecked());
    else if(act == waterSupplyAct)
        CtrConfig::setValueByName("application_settings.waterSupplyRasterVisible", act->isChecked());
    else if(act == heatingAct)
        CtrConfig::setValueByName("application_settings.heatingRasterVisible", act->isChecked());
    else if(act == electricityAct)
        CtrConfig::setValueByName("application_settings.electricityRasterVisible", act->isChecked());
    else if(act == doorsAct)
        CtrConfig::setValueByName("application_settings.doorsRasterVisible", act->isChecked());
    else if(act == defectsAct)
        CtrConfig::setValueByName("application_settings.defectsRasterVisible", act->isChecked());

    emit rastersVisibleChanged();
}

