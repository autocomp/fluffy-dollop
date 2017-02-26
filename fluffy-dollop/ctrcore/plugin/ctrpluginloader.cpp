#include "ctrpluginloader.h"

#include <QFile>
#include <QDir>
#include <QPluginLoader>

#include <ctrcore/ctrcore/ctrconfig.h>
#include <ctrcore/ctrcore/ctrdefines.h>

//unilog::LoggerPtr CtrPluginLoader::m_logger;

CtrPluginLoader::CtrPluginLoader()
{
//    m_logger = unilog::Unilog::instance()->getLogger(unilog::Unilog::FC_CORE, CTRPLUGIN);
    init();
}

CtrPlugin *CtrPluginLoader::getPlugin(TYPE_PLUGIN type, QString so_name)
{
    CtrPlugin* plugin = nullptr;
    QString so_path = m_base_path + m_path_map[type] + QDir::separator() + so_name;
    QFileInfo fi(so_path);
    QString so_abs_path = fi.absoluteFilePath();
    QFile file(so_abs_path);

    if(file.exists())
    {
        QPluginLoader loader(so_abs_path);
        QString err = loader.errorString();
        QObject* object(loader.instance());

        CtrPluginIface* pluginIface = qobject_cast<CtrPluginIface*>(object);
        if(pluginIface)
        {
            plugin = pluginIface->createPlugin();
            if(plugin)
                plugin->setObjectName(so_name);
        }
//        else
//            UNILOG_ERROR(m_logger) << "PLUGIN LOAD ERROR :" << so_path.toStdString().data()
//                                   << ", :" << loader.errorString().toStdString().data() << std::endl;
    }
    return plugin;
}

QList<CtrPlugin*> CtrPluginLoader::getPluginList(TYPE_PLUGIN type)
{
    QDir pluginsDir(QString(m_base_path).append("/").append(m_path_map[type]));
    QList<CtrPlugin*> list;

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        CtrPlugin* plugin = getPlugin(type, fileName);
        if(plugin)
            list.append(plugin);
    }
    return list;
}

void CtrPluginLoader::init()
{
    QString pluginsDir = CtrConfig::getValueByName(QString(CTR_SYSTEM_FILE_NAME).append(".plugins.rootdir")).toString();
    if(pluginsDir.isEmpty())
    {
        pluginsDir = "./plugins";
        CtrConfig::setValueByName(QString(CTR_SYSTEM_FILE_NAME).append(".plugins.rootdir"), pluginsDir);
    }
    setBasePath(pluginsDir);

    QStringList pluginTypes = CtrConfig::getStringListByName((QString(CTR_SYSTEM_FILE_NAME).append(".plugins.types")));
    if(pluginTypes.isEmpty())
    {
        pluginTypes << ctrplugin::getNameByType(TYPE_APP)
                    << ctrplugin::getNameByType(TYPE_OBJECT)
                    << ctrplugin::getNameByType(TYPE_PROVIDER)
                    << ctrplugin::getNameByType(TYPE_VISUALIZER);

        CtrConfig::setValueByName(QString(CTR_SYSTEM_FILE_NAME).append(".plugins.types"), pluginTypes);
    }

    QString location;

    foreach(QString type, pluginTypes)
    {
        location = CtrConfig::getValueByName(QString(CTR_SYSTEM_FILE_NAME).append(".plugins.location.").append(type)).toString();
        if(location.isEmpty())
        {
            CtrConfig::setValueByName(QString(CTR_SYSTEM_FILE_NAME).append(".plugins.location.").append(type), QString("./").append(type));
        }
        m_path_map.insert(ctrplugin::getTypeByName(type), location);
    }
}

void CtrPluginLoader::setBasePath(QString base_path)
{
    m_base_path = base_path;

    if(!m_base_path.endsWith('/'))
        m_base_path += '/';
}
