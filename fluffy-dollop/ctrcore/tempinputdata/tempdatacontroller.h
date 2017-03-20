#ifndef TEMPDATACONTROLLER_H
#define TEMPDATACONTROLLER_H

#include <QObject>
#include <QMap>
#include <QList>
#include "tempdatatypies.h"

class TempDataController : public QObject
{
    Q_OBJECT
public:
    static TempDataController *instance()
    {
        if(! _instance)
            _instance = new TempDataController();
        return _instance;
    }

    bool getRoomInfo(qulonglong id, temp_data::RoomInfo &data);
    bool getAreaInfo(qulonglong id, temp_data::AreaInfo &data);
    bool getTenant(qulonglong id, temp_data::Tenant & data);

    void  setAreaInfo(qulonglong id, temp_data::AreaInfo &data);

public slots:


signals:


private:
    static TempDataController * _instance;
    TempDataController();
    void readTenantData(const QString &xmlFile);
    void readRoomInfo(const QString &xmlFile);

    QMap<qulonglong, temp_data::RoomInfo> _roomInfoMap;
    QMap<qulonglong, temp_data::Tenant> _tenantMap;
    QMap<qulonglong, temp_data::AreaInfo> _areaMap;
};

#endif // TEMPDATACONTROLLER_H
