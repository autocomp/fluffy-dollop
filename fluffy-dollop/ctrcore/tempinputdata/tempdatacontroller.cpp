#include "tempdatacontroller.h"
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <ctrcore/ctrcore/ctrconfig.h>

TempDataController * TempDataController::_instance = nullptr;

TempDataController::TempDataController()
{
    QString destPath;
    QVariant regionBizInitJson_Path = CtrConfig::getValueByName(QString("application_settings.regionBizInitJson_Path"));
    if(regionBizInitJson_Path.isValid())
         destPath = QFileInfo(regionBizInitJson_Path.toString()).absolutePath() + QDir::separator();

    readTenantData(destPath + QString("tenant.xml"));
    readRoomInfo(destPath + QString("roominfo.xml"));
}

void TempDataController::readTenantData(const QString & xmlFile)
{
    bool ok;
    QDomDocument doc;
    QFile file(xmlFile);
    if(file.open(QIODevice::ReadOnly))
        ok = doc.setContent(file.readAll());
    file.close();
    if(ok)
    {
        QDomElement elem = doc.documentElement();
        QDomNode node = elem.firstChild();
        while(!node.isNull())
        {
            QDomElement subElement = node.toElement();
            qulonglong id = subElement.attribute("id").toULongLong();
            QString name = subElement.attribute("name");
            QString debt = subElement.attribute("debt");
            temp_data::Tenant tenant;
            tenant.name = name;
            if(debt.isEmpty() == false)
                tenant.debt = debt.toUInt();
            _tenantMap.insert(id, tenant);

            node = node.nextSibling();
        }
    }
}

void TempDataController::readRoomInfo(const QString & xmlFile)
{
    bool ok;
    QDomDocument doc;
    QFile file(xmlFile);
    if(file.open(QIODevice::ReadOnly))
        ok = doc.setContent(file.readAll());
    file.close();
    if(ok)
    {
        QDomElement elem = doc.documentElement();
        QDomNode node = elem.firstChild();
        while(!node.isNull())
        {
            QDomElement subElement = node.toElement();
            qulonglong id = subElement.attribute("id").toULongLong();
            double square = subElement.attribute("square").toDouble();
            int state = subElement.attribute("state").toInt();
            qulonglong tenant = subElement.attribute("tenant").toULongLong();
            temp_data::RoomInfo roomInfo;
            roomInfo.square = square;
            roomInfo.state = (temp_data::RoomState)state;
            roomInfo.tenant = tenant;
            _roomInfoMap.insert(id, roomInfo);

            node = node.nextSibling();
        }
    }
}

bool TempDataController::getRoomInfo(qulonglong id, temp_data::RoomInfo &data)
{
    auto it = _roomInfoMap.find(id);
    if(it != _roomInfoMap.end())
    {
        data = it.value();
        return true;
    }
    return false;
}

bool TempDataController::getAreaInfo(qulonglong id, temp_data::AreaInfo &data)
{
    auto it = _areaMap.find(id);
    if(it != _areaMap.end())
    {
        data = it.value();
        return true;
    }
    return false;
}

bool TempDataController::getTenant(qulonglong id, temp_data::Tenant &data)
{
    auto it = _tenantMap.find(id);
    if(it != _tenantMap.end())
    {
        data = it.value();
        return true;
    }
    return false;
}

void TempDataController::setAreaInfo(qulonglong id, temp_data::AreaInfo &data)
{
    _areaMap.insert(id, data);
}


