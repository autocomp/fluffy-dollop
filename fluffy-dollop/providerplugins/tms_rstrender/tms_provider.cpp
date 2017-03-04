#include "tms_provider.h"
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrrasterrender/tms/localdataprovider.h>

TmsProvider::TmsProvider()
{
}

TmsProvider::~TmsProvider()
{
}

void TmsProvider::instanceToFactory()
{
    data_system::LocalDataProvider *tmp_prov = new data_system::LocalDataProvider;
    data_system::DataProviderFactory::instance()->registerProvider((data_system::AbstractDataProvider*)tmp_prov, data_system::LocalDataProvider::createProvider);
    delete tmp_prov;
}

//Q_EXPORT_PLUGIN2(tms_ioplugin, TmsProviderInterface)

