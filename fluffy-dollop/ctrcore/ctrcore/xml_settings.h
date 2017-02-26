#ifndef __XML_SETTINGS_H__
#define __XML_SETTINGS_H__

#include <QMap>
#include <QString>
#include <QSettings>

class QDomDocument;
class QDomElement;
class QIODevice;

class XmlSettings
{
    QSettings * m_pSettings;
public:
    XmlSettings(QString fname = "");
    ~XmlSettings();

    inline QSettings& settings() const { return * m_pSettings; }

    // при использование XmlSettings::value конфиг создастся автоматически, если его не было
    QVariant value( const QString & key, const QVariant & defaultValue = QVariant() );
    void setValue ( const QString & key, const QVariant & value );
    bool isContain( const QString & key);

private:
    static bool readXmlFile(QIODevice &device, QSettings::SettingsMap &map);
    static bool writeXmlFile(QIODevice &device, const QSettings::SettingsMap &map);
    static void processWriteKey( QDomDocument& doc, QDomElement& domElement, QString key, const QVariant& value );
    static void processReadKey( QString key, QSettings::SettingsMap &map, QDomElement& domElement );
};

#endif // __XML_SETTINGS_H__
