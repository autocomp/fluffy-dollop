
#include "xml_settings.h"
#include <QDomDocument>
#include <iostream>
#include <QStringList>
#include <QDebug>

XmlSettings::XmlSettings( QString fname )
{
    if ( fname.isEmpty() )
    {
        std::cerr << "Empty file name";
        exit(1);
    }

    static const QSettings::Format XmlFormat = QSettings::registerFormat("xml", readXmlFile, writeXmlFile);
    m_pSettings = new QSettings( fname, XmlFormat );
}

XmlSettings::~XmlSettings()
{
    delete m_pSettings;
}

QVariant XmlSettings::value( const QString & key, const QVariant & defaultValue )
{
    if ( !settings().contains( key ) )
        settings().setValue( key, defaultValue );
    return settings().value( key );
}
void XmlSettings::setValue(const QString &key, const QVariant &value)
{
    settings().setValue(key, value);
}

bool XmlSettings::isContain(const QString &key)
{
    return settings().contains( key );
}

bool XmlSettings::readXmlFile(QIODevice &device, QSettings::SettingsMap &map)
{
    QDomDocument doc("");
    if (!doc.setContent(&device))
        return false;

    QDomElement root = doc.documentElement();
    processReadKey("", map, root);

    return true;
}

bool XmlSettings::writeXmlFile(QIODevice &device, const QSettings::SettingsMap &map)
{
    QDomDocument doc("");
    QDomElement root = doc.createElement("root");;
    doc.appendChild(root);

    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext())
    {
        i.next();

        QString  sKey = i.key();
        QVariant value = i.value();
        processWriteKey( doc, root, sKey, value );
    };

    QDomNode xmlNode = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.insertBefore(xmlNode, doc.firstChild());

    QTextStream out( &device );
    doc.save(out, 4);

    return true;
}

void XmlSettings::processWriteKey( QDomDocument& doc, QDomElement& domElement, QString key, const QVariant& value )
{
    int slashPos = key.indexOf( '.' );

    // переданный ключ является параметром
    if ( slashPos < 0 )
    {
        // не пишем в конфиг параметр size (является ограничением - нельзя исп. пар-тр с таким именем)
        if ( key == "size" ) return;

        QDomElement elem = doc.createElement(key);
        if( value.canConvert< QStringList >()
                && value.toStringList().size() > 1 )
        {
            // пишем, как лист
            QStringList lst = value.toStringList();
            for( QString str: lst )
                processWriteKey( doc, domElement, key, str );
        }
        else
        {
            // пишем, как строка
            QDomText text =  doc.createTextNode(value.toString());
            domElement.appendChild(elem);
            elem.appendChild(text);
        }

        return;
    };

    // получение имени группы соответствующей xml ноде
    QString groupName = key.left( slashPos );

    // если в качестве имени использован числовой параметр - это табличная строка, преобразуем ее в row_?
    if ( groupName.toInt() )
    {
        groupName = "row_" + groupName;
        domElement.toElement().setAttribute("table", "1");
    };

    // поиск/создание ноды соответствующей ключу
    QDomElement groupElement;

    QDomNode findedGroupNode = domElement.namedItem( groupName );

    if ( findedGroupNode.isNull() )
    {
        groupElement = doc.createElement( groupName );
        domElement.appendChild( groupElement );
    }
    else
        groupElement = findedGroupNode.toElement();

    // готовим обрезанную часть ключа
    key = key.right( key.size() - slashPos - 1 );

    // продолжение обработки (создание/поиск групп) - пока не найдется параметр
    processWriteKey( doc, groupElement, key, value );
}

void XmlSettings::processReadKey( QString key, QSettings::SettingsMap &map, QDomElement& domElement )
{
    QDomNamedNodeMap namedNodeMap = domElement.attributes();

    // Добавление всех атрибутов элемента в качестве значений
    for (int i = 0; i < namedNodeMap.count(); ++i)
    {
        QString name = namedNodeMap.item( i ).toAttr().name();
        QString value = namedNodeMap.item( i ).toAttr().value();
        map.insert( key + name, value );
    };

    if(!domElement.firstChild().toText().isNull()){
        QString value = domElement.firstChild().toText().data();
        QString key_str = key.left(key.size()-1);

        // ищем значение для наполнения списка
        QVariant map_value = QVariant();
        if( map.find( key_str ) != map.end() )
        {
            map_value = map.take( key_str );
        }

        if( map_value.canConvert< QStringList >() )
        {
            // пополняем список
            QStringList lst = map_value.toStringList();
            lst.append( value );
            map_value = lst;
        }
        else
        {
            // создаем список с нуля
            map_value = value;
        }

        map.insert( key_str, map_value );
    }

    QDomNodeList nlChild = domElement.childNodes();

    // если узел является таблицей - то все дети строки
    bool isTable = domElement.attribute("table", "0").toInt();
    // создаем доп. элемент size равный числу детей (необходим для QSettings - beginArray)
    if ( isTable )
        map.insert( key + "size", nlChild.count() );

    // проход по всем детям
    for (int i = 0; i < nlChild.count(); ++i)
    {
        QString childName = nlChild.item(i).toElement().tagName();
        if ( childName.contains("row_") )
            childName = childName.right(childName.size() - 4);

        QString subKey = key + childName + ".";
        QDomElement subElement = nlChild.item(i).toElement();
        processReadKey(subKey, map, subElement);
    };
}
