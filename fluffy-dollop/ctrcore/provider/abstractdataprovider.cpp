#include "abstractdataprovider.h"

using namespace data_system;

AbstractDataProvider::AbstractDataProvider()
{
    m_device = 0;
    m_providerURL.setScheme("file");
    m_providerURL.setPath(DEFAULT_URL);
}

AbstractDataProvider::~AbstractDataProvider()
{
    emit signalProviderDestroyed();

    if(m_device)
    {
        delete m_device;
    }
}

const QUrl &AbstractDataProvider::getProviderUrl() const
{
    return m_providerURL;
}

void AbstractDataProvider::setProviderUrl(const QUrl &url)
{
    m_providerURL = url;
}

uint AbstractDataProvider::getProviderId() const
{
    return m_id;
}

void AbstractDataProvider::setMetaData(const QString &key, const QVariant &value)
{
    m_metadata.setProperty(key, value, true);
}

const MetaData &AbstractDataProvider::metaData()
{
    return m_metadata;
}

QVariant AbstractDataProvider::getMetaData(const QString &key)
{
    return m_metadata.properties().value(key, QVariant());
}

MetaData::MetaData()
    : QObject()
{}

void MetaData::setProperty(const QString &name, const QVariant &prop, bool rewrite)
{
    QMap<QString, QVariant>::iterator map_it;

    map_it = m_properties.find(name);
    if(map_it == m_properties.end())
    {
        m_properties.insert(name, prop);
    }
    else
    {
        if(rewrite)
        {
            m_properties[name] = prop;
        }
    }
}

bool MetaData::property(QString name, QVariant &prop) const
{
    QMap<QString, QVariant>::const_iterator map_it;

    bool ret_code = true;

    map_it = m_properties.find(name);
    if(map_it == m_properties.end())
    {
        ret_code = false;
    }
    else
    {
        prop = *map_it;
    }

    return ret_code;
}

