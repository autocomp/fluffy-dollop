#ifndef COMMONPROVIDERDEVICE_H
#define COMMONPROVIDERDEVICE_H

#include "abstractproviderdevice.h"

namespace data_system
{
class CommonProviderDevice : public AbstractProviderDevice
{
QStringList m_methods_list;

MetaData m_md;

public:
    CommonProviderDevice();

    virtual QStringList methods() const
    {
        return m_methods_list;
    }

    virtual QIODevice *getDevice(const QUrl &url, AbstractDataProvider::WorkMode mode = AbstractDataProvider::Both);

    virtual const MetaData &metaData();

    static AbstractProviderDevice *createCommonDevice()
    {
        return new CommonProviderDevice();
    }
};
}

#endif // COMMONPROVIDERDEVICE_H
