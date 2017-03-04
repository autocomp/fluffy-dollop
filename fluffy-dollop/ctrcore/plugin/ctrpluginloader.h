#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include "ctrplugin.h"
//#include <unilog/unilog.h>

#include <QString>
#include <QMap>

using namespace ctrplugin;

class CtrPluginLoader
{

public:
    /**
     * @brief getPlugin - метод возвращает плагин по его типу и имени библиотеки
     * @param type - тип плагина - объявлены в ctrplugindefines
     * @param so_name - имя библиотеки (libname.so)
     * @return
     */
    CtrPlugin* getPlugin(TYPE_PLUGIN type, QString so_name);

    /**
     * @brief getPlugins - метод возвращает список плагинов заданного типа
     * @param type
     * @return
     */
    QList<CtrPlugin*> getPluginList(TYPE_PLUGIN type);

    static CtrPluginLoader* instance()
    {
        static CtrPluginLoader loader;
        return &loader;
    }


private:
    CtrPluginLoader();
    void init();
    void setBasePath(QString base_path);
    QString m_base_path;
    QMap <TYPE_PLUGIN, QString> m_path_map;

    /// Смартпоинтер на логгер
//    static unilog::LoggerPtr m_logger;



};

#endif // PLUGINLOADER_H
