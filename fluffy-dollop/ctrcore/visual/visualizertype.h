#ifndef VISUALIZERTYPE_H
#define VISUALIZERTYPE_H

#include <QMap>
#include <QVariant>

namespace visualize_system
{

enum VisualizerType
{
    // значения можно объединять в битовую маску
    Invalid         = 0x0, // 0000
    Visualizer2D    = 0x1, // 0001
    Visualizer3D    = 0x2, // 0010
    VisualizerPixel = 0x4, // 0100
    VisualizerVideo = 0x8  // 1000
};

enum class ProviderType
{
    BaseCover,
    Images,
    BaseCoverAndImages
};

class VisualizerCommand
{
public:
    VisualizerCommand(QString commandName) : _commandName(commandName) {}

    void setProperty(const QString& propertyName, const QVariant& propertyValue)
    {
        _properties.insert(propertyName, propertyValue);
    }

    bool getProperty(QString propertyName, QVariant & propertyValue) const
    {
        auto it = _properties.find(propertyName);
        if(it != _properties.end())
        {
           propertyValue = it.value();
           return true;
        }
        else
            return false;
    }

protected:
    const QString _commandName;
    QMap<QString, QVariant> _properties;
};

}

#endif
