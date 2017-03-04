#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include "visualizerinterface.h"
#include "properties.h"
//#include "processingsettingsholder.h"
#include <ctrcore/provider/rasterdataprovider.h>

namespace visualize_system
{

class DataInterface : public VisualizerInterface
{
    Q_OBJECT
public:
    explicit DataInterface(AbstractVisualizer& abstractVisualizer);

    // работа с провайдерами.
    QList<uint> addBaseProviders(const QList<uint> & providers);
    QList<uint> addProviders(const QList<uint> & providers);
    QList<uint> removeProviders(const QList<uint> & providers);
    QList<uint> getProviders(ProviderType providerType = ProviderType::BaseCoverAndImages);

    // работа со свойствами визуализатора.
    bool getVisualizerProperty(QString propertyName, QVariant& property);
    void setVisualizerProperty(QString propertyName, const QVariant& property);
    QMap<QString, QVariant> getVisualizerProperties();

    // работа со свойствами провайдеров.
    bool getProviderViewProperty(uint providerId, QString propertyName, QVariant& property);
    void setProviderViewProperty(uint providerId, QString propertyName, const QVariant& property);
    QMap<QString, QVariant> getProviderViewProperties(uint providerId);

    // работа с параметрами обработок провайдеров.
//    void setProviderProcessingSettings(uint providerId, const QString& processingSystemName, QSharedPointer<processing_system::ProcessingSettings> settings);
//    bool eraseProviderProcessingSettings(uint providerId, const QString& processingSystemName);
//    QSharedPointer<processing_system::ProcessingSettings> getProviderProcessingSettings(uint providerId, const QString& processingSystemName);
//    QStringList getProviderProcessingSystemNames(uint providerId);
//    QStringList getProviderProcessingUserNames(uint providerId);
//    QString getProviderProcessingCacheKey(uint providerId);

signals:
    void signalProvidersAdded(const QList<uint> & providers);
    void signalBaseProvidersAdded(const QList<uint> & providers);
    void signalProvidersRemoved(const QList<uint> & providers);

    void signalVisualizerPropertyChanged(QString propertyName);
    void signalProviderPropertyChanged(uint providerId, QString propertyName);
    void signalProviderProcessingSettingsChanged(uint providerId, QString processingSystemName);

    void signalProviderMatched(uint id);

protected:
    Properties _visualizerProperties;
    QMap<uint, Properties> _providerProperties;
//    ProcessingSettingsHolder _processingSettingsHolder;
};
}
#endif // DATAINTERFACE_H







