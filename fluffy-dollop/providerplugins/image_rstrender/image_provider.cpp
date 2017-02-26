#include "image_provider.h"
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrrasterrender/image/imageprovider.h>

ImgProvider::ImgProvider()
{
}

ImgProvider::~ImgProvider()
{
}

void ImgProvider::instanceToFactory()
{
    data_system::ImageProvider *tmp_prov = new data_system::ImageProvider;
    data_system::DataProviderFactory::instance()->registerProvider((data_system::AbstractDataProvider*)tmp_prov, data_system::ImageProvider::createProvider);
    delete tmp_prov;
}

//Q_EXPORT_PLUGIN2(image_ioplugin, ImgProviderInterface)
