#include "datainterface.h"

using namespace visualize_system;

DataInterface::DataInterface(AbstractVisualizer& abstractVisualizer)
    : VisualizerInterface(abstractVisualizer)
{
}

QList<uint> DataInterface::addBaseProviders(const QList<uint> & providers)
{
    QList<uint> list = _abstractVisualizer.addBaseProviders(providers);
    if(list.isEmpty() == false)
    {
        foreach(uint providerId, list)
            _providerProperties.insert(providerId, Properties());

        emit signalBaseProvidersAdded(list);
    }
    return list;
}

QList<uint> DataInterface::addProviders(const QList<uint> & providers)
{
    QList<uint> list = _abstractVisualizer.addProviders(providers);
    if(list.isEmpty() == false)
    {
        foreach(uint providerId, list)
            _providerProperties.insert(providerId, Properties());

        emit signalProvidersAdded(list);
    }
    return list;
}

QList<uint> DataInterface::removeProviders(const QList<uint> & providers)
{
    QList<uint> list = _abstractVisualizer.removeProviders(providers);
    if(list.isEmpty() == false)
    {
        foreach (uint id, list)
        {
//            _processingSettingsHolder.removeProvider(id);
            auto it = _providerProperties.find(id);
            if(it != _providerProperties.end())
                _providerProperties.erase(it);
        }
        emit signalProvidersRemoved(list);
    }
    return list;
}

QList<uint> DataInterface::getProviders(ProviderType providerType)
{
    return _abstractVisualizer.getProviders(providerType);
}

bool DataInterface::getVisualizerProperty(QString propertyName, QVariant& property)
{
    return _visualizerProperties.getProperty(propertyName, property);
}

void DataInterface::setVisualizerProperty(QString propertyName, const QVariant& property)
{
    _visualizerProperties.setProperty(propertyName, property);
    emit signalVisualizerPropertyChanged(propertyName);
}

QMap<QString, QVariant> DataInterface::getVisualizerProperties()
{
     return _visualizerProperties.getProperties();
}

bool DataInterface::getProviderViewProperty(uint providerId, QString propertyName, QVariant& property)
{
    auto it = _providerProperties.find(providerId);
    if(it != _providerProperties.end())
        return it.value().getProperty(propertyName, property);
    else
        return false;
}

void DataInterface::setProviderViewProperty(uint providerId, QString propertyName, const QVariant& property)
{
    auto it = _providerProperties.find(providerId);
    if(it != _providerProperties.end())
    {
        it.value().setProperty(propertyName, property);
        emit signalProviderPropertyChanged(providerId, propertyName);
    }
}

QMap<QString, QVariant> DataInterface::getProviderViewProperties(uint providerId)
{
    auto it = _providerProperties.find(providerId);
    if(it != _providerProperties.end())
        return it.value().getProperties();
    else
        return QMap<QString,QVariant>();
}

//void DataInterface::setProviderProcessingSettings(uint providerId, const QString& processingSystemName, QSharedPointer<processing_system::ProcessingSettings> settings)
//{
//    _processingSettingsHolder.setProviderProcessingSettings(providerId, processingSystemName, settings);
//    emit signalProviderProcessingSettingsChanged(providerId, processingSystemName);
//}

//bool DataInterface::eraseProviderProcessingSettings(uint providerId, const QString& processingSystemName)
//{
//    bool RES = _processingSettingsHolder.eraseProcessingSettings(providerId, processingSystemName);;
//    if(RES)
//        emit signalProviderProcessingSettingsChanged(providerId, processingSystemName);
//    return RES;
//}

//QSharedPointer<processing_system::ProcessingSettings> DataInterface::getProviderProcessingSettings(uint providerId, const QString& processingSystemName)
//{
//    return _processingSettingsHolder.getProviderProcessingSettings(providerId, processingSystemName);
//}

//QStringList DataInterface::getProviderProcessingSystemNames(uint providerId)
//{
//    return _processingSettingsHolder.getProviderProcessingSystemNames(providerId);
//}

//QStringList DataInterface::getProviderProcessingUserNames(uint providerId)
//{
//    return _processingSettingsHolder.getProviderProcessingUserNames(providerId);
//}

//QString DataInterface::getProviderProcessingCacheKey(uint providerId)
//{
//    return _processingSettingsHolder.getProviderProcessingCacheKey(providerId);
//}

