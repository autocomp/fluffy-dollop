#include "commonproviderdevice.h"

#include <QFile>

using namespace data_system;

CommonProviderDevice::CommonProviderDevice()
{
    m_methods_list << "file";
}

QIODevice *CommonProviderDevice::getDevice(const QUrl &url, AbstractDataProvider::WorkMode mode)
{
    QIODevice *ret_dev = 0;
    QIODevice::OpenMode open_mode;

    if(url.scheme() == "file")
    {
        QFile *f = new QFile;
        f->setFileName(url.path());
        ret_dev = f;
    }

    if(ret_dev)
    {
        switch(mode)
        {
        case AbstractDataProvider::Read:
            open_mode |= QIODevice::ReadOnly;
            break;
        case AbstractDataProvider::Write:
            open_mode |= QIODevice::WriteOnly;
            break;
        case AbstractDataProvider::Both:
            open_mode |= QIODevice::ReadWrite;
            break;
        default:
            break;
        }

        if(!ret_dev->open(open_mode))
        {
            delete ret_dev;
            ret_dev = 0;
        }
    }

    return ret_dev;
}

const MetaData &CommonProviderDevice::metaData()
{
    return m_md;
}

