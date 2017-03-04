#include "dataproviderfactory.h"
//#include "rasterdataprovider.h"
#include <QFileInfo>
#include <QDebug>

#include "commonproviderdevice.h"

using namespace data_system;

DataProviderFactory *DataProviderFactory::m_fact = 0;

DataProviderFactory::DataProviderFactory()
{
    m_id_counter = 0;
    registerDevice(new CommonProviderDevice, CommonProviderDevice::createCommonDevice);
}

QSharedPointer<AbstractDataProvider> DataProviderFactory::createProvider(const QUrl &url, QString source, AbstractDataProvider::WorkMode mode)
{
    QSharedPointer<AbstractDataProvider> ptr;
    auto itUrl = m_url_to_id.find(url);
    if(itUrl != m_url_to_id.end())
    {
        uint id = itUrl.value();
        ptr = getProvider(id);
    }
    else
    {
        if(source.isEmpty())
        {
            source = getSourceFromUrl(url);
        }

        if(!source.isEmpty())
        {
            auto prov_it = m_providers.find(source);
            if(prov_it != m_providers.end())
            {
                ProviderData dat = *prov_it;
                AbstractDataProvider *ret_prov = dat.create();
                connect(ret_prov, SIGNAL(signalProviderDestroyed()), this, SLOT(slotProviderDestroyed()));
                connect(ret_prov, SIGNAL(signalProviderSaved()), this, SLOT(slotProviderSaved()));
                connect(ret_prov, SIGNAL(signalProviderChanged()), this, SLOT(slotProviderChanged()));
                ret_prov->setDevice(getDevice(url));
                ret_prov->setProviderUrl(url);

                m_id_counter += 1;
                if(m_id_counter == 0)
                    m_id_counter = 1;
                uint id = m_id_counter;
                ret_prov->m_id = id;
                m_url_to_id.insert(url, id);

                ptr = QSharedPointer<AbstractDataProvider>(ret_prov);
                QWeakPointer<AbstractDataProvider>w_ptr(ptr.toWeakRef());
                m_id_to_provider.insert(id, w_ptr);
            }
        }
    }

    return ptr;
}


QSharedPointer<AbstractDataProvider> DataProviderFactory::getProvider(uint id)
{
    QSharedPointer<AbstractDataProvider> ptr;

    auto itId = m_id_to_provider.find(id);
    if(itId != m_id_to_provider.end())
    {
        QWeakPointer<AbstractDataProvider> w_ptr = itId.value();
        if(w_ptr.isNull())
        {
            m_id_to_provider.erase(itId);
        }
        else
        {
            ptr = w_ptr.toStrongRef();
        }
    }
    return ptr;
}

void DataProviderFactory::slotProviderDestroyed()
{
    AbstractDataProvider *adp = dynamic_cast<AbstractDataProvider*>(sender());
    if(adp)
    {
        uint visualSystemRasterId = adp->getProviderId();

        uint64_t objreprRasterId(0);
//        RasterDataProvider *rdp = dynamic_cast<RasterDataProvider*>(adp);
//        if(rdp)
//            objreprRasterId = rdp->getObjreprRasterId();

        emit providerDestroyed(visualSystemRasterId, objreprRasterId);

        auto itId = m_id_to_provider.find(visualSystemRasterId);
        if(itId != m_id_to_provider.end())
        {
            m_id_to_provider.erase(itId);
//            qDebug() << "DataProviderFactory::providerDestroyed, erase :" << id;
        }

        for(auto itUrl = m_url_to_id.begin(); itUrl != m_url_to_id.end(); ++itUrl)
            if(itUrl.value() == visualSystemRasterId)
            {
                m_url_to_id.erase(itUrl);
                break;
            }
    }
    //    qDebug() << "DataProviderFactory::providerDestroyed, m_id_to_provider.size() :" << m_id_to_provider.size() << ", m_url_to_id.size() :" << m_url_to_id.size();
}

void DataProviderFactory::slotProviderSaved()
{
    AbstractDataProvider *adp = dynamic_cast<AbstractDataProvider*>(sender());
    if(adp)
    {
        uint id = adp->getProviderId();
        for(auto itUrl = m_url_to_id.begin(); itUrl != m_url_to_id.end(); ++itUrl)
            if(itUrl.value() == id)
            {
                m_url_to_id.erase(itUrl);
                m_url_to_id.insert(adp->getProviderUrl(), id);

                emit providerSaved(id);

                break;
            }
    }
}


void DataProviderFactory::slotProviderChanged()
{
    AbstractDataProvider *adp = dynamic_cast<AbstractDataProvider*>(sender());
    if(adp)
    {
        uint id = adp->getProviderId();
        emit providerChanged(id);
    }
}

void DataProviderFactory::registerProvider(const AbstractDataProvider *adp, AbstractDataProvider::createFucntion f)
{
    foreach(QString resousce, adp->resourceType())
    {
        ProviderData dat = {f, adp->workMode()};
        m_providers.insert(resousce, dat);
        m_resourceType << resousce;

        QList<InParam> tmp_list;
        foreach(QString key, adp->outKeys().keys()) {
            if(!adp->outKeys().value(key).isEmpty())
            {
                m_ext_to_driver.insert(key, adp->outKeys().value(key));

                tmp_list = adp->outParameters(adp->outKeys().value(key)[0]);
                if(!tmp_list.isEmpty())
                {
                    m_driver_to_param.insert(adp->outKeys().value(key)[0], tmp_list);
                }
            }
        }
    }
}

QIODevice *DataProviderFactory::getDevice(const QUrl &url)
{
    QIODevice *ret_dev = 0;
    DeviceMap::iterator it = m_devices.find(url.scheme());
    if(it != m_devices.end())
    {
        AbstractProviderDevice *dev = it.value()();

        if(dev)
        {
            ret_dev = dev->getDevice(url);
        }
    }

    return ret_dev;
}

void DataProviderFactory::registerDevice(const AbstractProviderDevice *dev, AbstractProviderDevice::createFunction func)
{
    foreach(QString method, dev->methods()) {
        m_devices.insert(method, func);
    }
}

QString DataProviderFactory::getSourceFromUrl(const QUrl &url)
{
    QString source;

    if(url.isLocalFile())
    {
        QFileInfo fi(url.path());
        source = fi.suffix();
    }
    else
    {  
        source = url.path();
    }

    source = QLocale::system().toLower(source);

    return source;
}

QStringList DataProviderFactory::driverByExt(const QString &ext)
{
    QStringList list;
    QString lowerExt = ext.toLower();
    if(m_ext_to_driver.contains(lowerExt))
    {
        list = m_ext_to_driver.value(lowerExt);
    }
    else
    {
        QStringList();
    }

    return list;
}

QList<InParam> DataProviderFactory::getParamByDriver(const QString &driver)
{
    QList<InParam> tmp_list;
    if(m_driver_to_param.contains(driver))
    {
        tmp_list = m_driver_to_param.value(driver);
    }

    return tmp_list;
}

uint DataProviderFactory::getNextId()
{
    m_id_counter += 1;
    if(m_id_counter == 0)
        m_id_counter = 1;
    return m_id_counter;
}
