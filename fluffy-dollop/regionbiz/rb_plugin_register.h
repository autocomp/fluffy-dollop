#ifndef RB_PLUGIN_REGISTER_H
#define RB_PLUGIN_REGISTER_H

/**
 * This macro register plugin with empty metadata
 * for QPluginLoader. Must be used on header (for moc)
 **/
#define REGISTER_PLUGIN( plugin ) \
    class plugin: public QObject \
    { \
        Q_OBJECT \
        Q_PLUGIN_METADATA(IID "ru.vira.autocomp.RegionBizPluginIface") \
    };

#endif // RB_PLUGIN_REGISTER_H
