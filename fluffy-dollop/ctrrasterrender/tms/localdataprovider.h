#ifndef LOCALDATAPROVIDER_H
#define LOCALDATAPROVIDER_H

#include "tmsdataprovider.h"
#include "tmsbaseadapter.h"
#include "googleadapter.h"

/**
 *  Класс унаследованный от абстрактного тмс-провайдера,
 *  переопределяет методы формирования запроса для получения данных от тмс-сервера организации.
 */

namespace data_system
{

class LocalDataProvider : public TMSDataProvider
{
    Q_OBJECT

public:
    LocalDataProvider();
    virtual ~LocalDataProvider() {}

    virtual bool open(const QUrl& url, WorkMode mode = Both);
    virtual bool reOpen(const QUrl &url, WorkMode mode = Both) {return false;}
    virtual bool saveAs(const QUrl &url, WorkMode mode = Both) {return false;}
    bool toFormat(const QUrl& url, const QString& type) {return true;}

    virtual void  setMinZoomLevel(uint minZlevel);
    virtual void  setMaxZoomLevel(uint maxZlevel);
    virtual void  setRetryLimit(uint retryLimit);
    virtual void  setTimeoute(uint timeouteSec);
    virtual void  setScaleForBottomLevels(bool on_off);
    virtual void  setLoadersLimit(int loadersLimit);
    virtual const TmsBaseAdapter * getTmsAdapter();

    uint virtual minZoomLevel()const;
    uint virtual maxZoomLevel()const;
    uint virtual retryLimit()const;
    uint virtual timeouteSec()const;
    bool virtual scaleForBottomLevels()const;
    int virtual loadersLimit()const;

    static AbstractDataProvider* createProvider()
    {
        return new LocalDataProvider();
    }

protected:
    virtual QString host() const;
    virtual QString serverPath() const;
    virtual int serverPort() const;
    virtual QString scheme() const;
    virtual QUrl query(int x, int y, int z) const;

private:
    TmsBaseAdapter * _localTmsAdapter;
};

}
#endif
