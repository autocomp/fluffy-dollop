#include "ctrconfig.h"
#include "ctrdefines.h"

#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>
#include <QRegExp>
#include <QDebug>

QMap<QString, QVariant> CtrConfig::m_custom_data;

QMap<QString, QString> CtrConfig::m_argumentsList;
QString CtrConfig::m_flagList;
QString CtrConfig::_application_name;
ParamsObserver CtrConfig::_params_observer;
int CtrConfig::_params_observer_delay;
int CtrConfig::_params_observer_verbose;

//unilog::LoggerPtr CtrConfig::_logger;

CtrConfig::CtrConfig()
{
    setInited(false);
}

CtrConfig::~CtrConfig()
{
    qDeleteAll(_settings_common);
    qDeleteAll(_settings_user);
//    UNILOG_DEBUG (_logger) << "Destroy CtrConfig" << std::endl;
}

void CtrConfig::setValueByName(QString key, QVariant val)
{
    if(!CtrConfig::getConfig()->_is_init)
    {
//        UNILOG_ERROR(_logger) << "Config manager wasn\'t init. Can't to set value" << std::endl;
        return;
    }

    int slahPos = key.indexOf(".");
    QString file = key.left(slahPos);
    key = key.right(key.size() - slahPos - 1);

    bool is_common_mode(true);
    bool is_set(true);
    XmlSettings* set = getConfig()->getFileByName(file, !is_common_mode, is_set);
    if(set)
        set->setValue(key, val);
//    else
//        UNILOG_ERROR (_logger) <<  "Config manager can\'t create file: " << key.toStdString().data() << std::endl;
}

QVariant CtrConfig::getValueByNameDef(QString func, QString file_name, int line,
                                      QString key, QVariant default_val, bool force_write )
{
    if(!CtrConfig::getConfig()->_is_init)
    {
//        UNILOG_ERROR (_logger) << "Config manager wasn\'t init. Can't to get value " << key.toStdString() << std::endl;
        return QVariant();
    }

    // push data of function call, if needed
    if( _params_observer_verbose > 0 )
    {
        bool is_cmd( true );
        bool new_item = _params_observer.checkFunctionInMap( key, !is_cmd );
        if ( new_item )
            _params_observer.addFuncFileLine( key, func, file_name, line );
    }

    int slahPos = key.indexOf(".");
    QString file = key.left(slahPos);
    key = key.right(key.size() - slahPos - 1);

    bool is_common_mode(true);
    bool is_set(true);

    // Если пользовательский существует
    if(getConfig()->getFileByName(file, !is_common_mode, !is_set))
    {
        if(!getConfig()->getFileByName(file, !is_common_mode, !is_set)->isContain(key))
        {
            // Если в пользовательском файле отсутствует поле извлекаем из системного ниже
            if(!getConfig()->getFileByName(file, is_common_mode, !is_set))
            {
                // если нужно, записываем в файл значение по умолчанию
                if( force_write )
                    setValueByName( file + '.' + key, default_val );

                // В системной отсутствует, возвращаем значение по умолчанию
                return default_val;
            }
        }
        else
        {
            // Если в пользовательском содержится значение
            return getConfig()->getFileByName(file, !is_common_mode, !is_set)->value(key, default_val);
        }
    }

    //Ищем значение из системного файла
    if(getConfig()->getFileByName(file, is_common_mode, !is_set))
    {
        if(getConfig()->getFileByName(file, is_common_mode, !is_set)->isContain(key))
            //Берем значение в системном файле
            return getConfig()->getFileByName(file, is_common_mode, !is_set)->value(key, default_val);
    }

    // проверяем, есть ли пользовательский (в случае, если нет значения ни в одном)
    XmlSettings* set = (getConfig()->getFileByName(file, !is_common_mode, !is_set));
    if(!set)
    {
        // если нужно, записываем в файл значение по умолчанию
        if( force_write )
            setValueByName( file + '.' + key, default_val );
//        else
//            UNILOG_ERROR (_logger) << "Can\'t open user file and don\'t find in common file, return default value of attr: "
//                                   << file.toStdString().data() << " "
//                                   << key.toStdString().data() << std::endl;
        return default_val;
    }

    return set->value(key, default_val);
}

QStringList CtrConfig::getStringListByNameDef( QString func, QString file_name, int line,
                                               QString key, QVariant default_val )
{
    QVariant value = getValueByNameDef( func, file_name, line, key, default_val ).toStringList();

    if( value.canConvert< QStringList >() )
        return value.toStringList();

    return QStringList();
}

bool CtrConfig::isValueByNamePresent(QString key)
{
    if(!CtrConfig::getConfig()->_is_init)
    {
//        UNILOG_ERROR(_logger) << "Config manager wasn\'t init. Can't to check value" << std::endl;
        return false;
    }

    int slahPos = key.indexOf(".");
    QString file = key.left(slahPos);
    key = key.right(key.size() - slahPos - 1);

    bool is_common_mode(true);
    bool is_set(true);

    XmlSettings* set;
    if( set = getConfig()->getFileByName( file, !is_common_mode, !is_set ))
    {
        // проерка в пользовательском файле
        return set->isContain( key );
    }
    else if( set = getConfig()->getFileByName( file, is_common_mode, !is_set ))
    {
        // проверка в системном файле
        return set->isContain( key );
    }
    else
        return false;
}

bool CtrConfig::checkConfigs()
{
    getConfig();
    if(!m_argumentsList.isEmpty())
    {
//        UNILOG_INFO(_logger) << "application started with the next parametrs ( total count):"
//                             << m_argumentsList.count()
//                             << std::endl;

        QMap<QString, QString>::const_iterator i = m_argumentsList.constBegin();
        while (i != m_argumentsList.constEnd())
        {
//            UNILOG_INFO(_logger) << i.key().toStdString().data() <<"="<< i.value().toStdString().data() << std::endl;
            ++i;
        }
    }

//    UNILOG_INFO(_logger) << "flags set is: " << m_flagList.toStdString().data() << std::endl;


    initParametersObserver();

    QString common_settings_path = ctrconfig::getCommonConfigPath(_application_name);
    qDebug() << common_settings_path;
    QString user_settings_path = ctrconfig::getUserConfigPath(_application_name);

//    UNILOG_INFO(_logger) << "application common config path is: "
//                         << common_settings_path.toStdString().data()
//                         << " application user config path is: "
//                         << user_settings_path.toStdString().data() << std::endl;

    QDir dir_common(common_settings_path);
    QDir dir_user(user_settings_path);

    bool res(false), is_common(true);

    if(!dir_common.exists())
    {
//        UNILOG_WARNING(_logger) << "common config path doesn\'t exist, need to create: "
//                                << dir_common.path().toStdString().data() << std::endl;
    }
    else if(dir_common.isReadable() && (!dir_common.entryInfoList(QDir::Files).isEmpty()))
        res = getConfig()->initialize(dir_common.entryInfoList(QDir::Files), is_common);
//    else
//        UNILOG_WARNING(_logger) << "common config path is empty or not accessible" << std::endl;

    if(!dir_user.exists())
    {
//        UNILOG_WARNING(_logger) << "user config path doesn\'t exist, need to create "
//                                << dir_user.path().toStdString().data() << std::endl;
    }
    else if(dir_user.isReadable() && (!dir_user.entryInfoList(QDir::Files).isEmpty()))
        res |= getConfig()->initialize(dir_user.entryInfoList(QDir::Files), !is_common);
//    else
//        UNILOG_WARNING(_logger) << "user config path is empty or not accessible" << std::endl;

    CtrConfig::getConfig()->setInited(true);

    return res;
}

bool CtrConfig::initialize(const QFileInfoList& confFileList,  bool is_common_mode)
{
    QFileInfo fInfo;
    bool res(false);
    foreach (fInfo, confFileList)
    {
        QRegExp rx("*.xml");
        rx.setPatternSyntax( QRegExp::Wildcard );

        if( fInfo.isReadable() )
        {
            if( rx.exactMatch( fInfo.fileName() ))
            {
                addNewFile(fInfo.baseName(), fInfo.filePath(), is_common_mode);
                res = true;
            }
//            else
//                UNILOG_WARNING(_logger) << "config file "
//                                        << fInfo.filePath().toStdString().data()
//                                        << "in "
//                                        << (is_common_mode ? "common" : "user")
//                                        << " folder have incorrect format"
//                                        << std::endl;
        }
//        else
//            UNILOG_WARNING(_logger) << "config file "
//                                    << fInfo.filePath().toStdString().data()
//                                    << "in "
//                                    << (is_common_mode ? "common" : "user")
//                                    << " folder is not readable"
//                                    << std::endl;
    }
    return res;
}

void CtrConfig::addNewFile(QString name, QString fname, bool is_common_mode)
{
    if(is_common_mode)
        _settings_common[name] = new XmlSettings(fname);
    else
        _settings_user[name] = new XmlSettings(fname);

//    UNILOG_DEBUG(_logger) << "config file "
//                          << fname.toStdString().data()
//                          << " has read to "
//                          << (is_common_mode ? "common" : "user")
//                          << " section" << std::endl;
}

XmlSettings* CtrConfig::getFileByName(QString name, bool is_common_mode, bool is_set)
{
    SettinsMap* settings = is_common_mode ? &_settings_common : &_settings_user;
    if(!settings->contains(name))
    {
        if(is_common_mode)
        {
//            UNILOG_WARNING(_logger) << "missing config common file: "
//                                    << name.toStdString().data()
//                                    << " Need to create. "
//                                    << (is_set ? "Try" : "Need")
//                                    << std::endl;
        }
        else
        {
//            UNILOG_WARNING(_logger) << "missing config user file:"
//                                    << name.toStdString().data()
//                                    << " Need to create. "
//                                    << (is_set ? "Try" : "Need")
//                                    << std::endl;
        }
        if(is_set)
        {
            if(CtrConfig::getAppName().isEmpty())
            {
//                UNILOG_WARNING(_logger)<< "Application name is empty, try to checkConfigs() with actual app name" << std::endl;
                return NULL;
            }

            QString common_settings_path = ctrconfig::getCommonConfigPath(_application_name);
            QString user_settings_path = ctrconfig::getUserConfigPath(_application_name);

            QDir dir(is_common_mode ? common_settings_path : user_settings_path);

            if(!dir.exists())
            {
                if(!is_common_mode)
                    if(ctrconfig::CreateUserConfigPath(_application_name))
                    {
//                        UNILOG_WARNING(_logger) << "can\'t create user config folder" << std::endl;
                    }
                    else
                    {
                    }
            }

            QString path = (is_common_mode ? common_settings_path : user_settings_path) + "/" + name + ".xml";
            
//            UNILOG_DEBUG(_logger) << "CONFIG FILE PATH" << std::endl;

            QFile file(path);
            if(file.open(QFile::WriteOnly))
            {
                file.close();
//                UNILOG_WARNING(_logger) << "config file "
//                                        << name.toStdString().data()
//                                        << " was created" << std::endl;
                addNewFile(name, path, is_common_mode);
            }
            else
                return NULL;
        }
        else
            return NULL;
    }
    return settings->value(name);
}

CtrConfig *CtrConfig::getConfig()
{
    static CtrConfig* inst = 0;
    if(inst == 0)
    {
        inst = new CtrConfig;

        QFileInfo fInf(qApp->applicationFilePath());
        _application_name = fInf.baseName();
//        _logger = unilog::Unilog::instance()->getLogger(unilog::Unilog::FC_CORE, CTRCORE);

    }
    return inst;
}

QString CtrConfig::getUserConfigPath()
{
    return ctrconfig::getUserConfigPath(getAppName());
}

void CtrConfig::initParametersObserver()
{
    if( _params_observer.timerStarted() )
        return;

    // init verbose
    if( m_argumentsList.contains( PARAMETERS_OBSERVE ))
        _params_observer_verbose = m_argumentsList[PARAMETERS_OBSERVE].toInt();
    else
        _params_observer_verbose = 0;

    // init delay
    if( m_argumentsList.contains( PARAMETERS_OBSERVE_DELAY ))
        _params_observer_delay = m_argumentsList[PARAMETERS_OBSERVE_DELAY].toInt();
    else
        _params_observer_delay = PARAMETERS_OBSERVE_DELAY_DEFAULT;

    // start timer
    if( _params_observer_verbose > 0
            && _params_observer_delay > 0 )
    {
        _params_observer.setVerbose( _params_observer_verbose );
        _params_observer.startTimerToPrint( _params_observer_delay );
        _params_observer.setArgs( &m_argumentsList );
    }
}

void CtrConfig::setCustomData(QString key,
                              QVariant value)
{
    m_custom_data.insert(key, value);
}

QVariant CtrConfig::getCustomData(QString key,
                                  QVariant default_value)
{
    if(m_custom_data.contains(key))
    {
        return m_custom_data.value(key);
    }
    else
    {
        return default_value;
    }
}

void CtrConfig::parseCmdLine()
{
    getConfig();
    QStringList appArgs = QCoreApplication::arguments();

    appArgs.removeFirst();

    for(uint i =0 ; i < appArgs.count(); i++ )
    {
        QString args = appArgs[i];

        if(args.contains("--", Qt::CaseSensitive) && (args.length() > 2))
        {
            QString name = args.mid(2);
            QString value;

            if(args.contains("=", Qt::CaseSensitive))
            {
                value = args.section("=",1);
                name.truncate(name.indexOf("="));

            }
            else if(args != appArgs.last())
            {
                i++;
                value = appArgs[i];
            }
            m_argumentsList.insert(name, value);

        }
        else if(args.startsWith("-") && (args.length() >= 2))
        {
            QString flags = args.mid(1);
            foreach(QChar simb, flags)
            {
                if(!m_flagList.contains(simb))
                    m_flagList.append(simb);

            }
        }
    }
}

bool CtrConfig::isCmdLineFlagPresent(QChar flag)
{
    if(!CtrConfig::getConfig()->_is_init)
    {
//        UNILOG_DEBUG(_logger) << "Config manager wasn\'t init. Can't to check flag" << std::endl;
        return false;
    }

    if(m_flagList.contains(flag))
        return true;
    return false;
}

QString CtrConfig::getCmdLineValueDef(const QString& name, bool& flag,
                                      QString func, QString file, int line)
{
    // push data of function call, if needed
    if( _params_observer_verbose > 0 )
    {
        bool new_item = _params_observer.checkFunctionInMap( name );
        if ( new_item )
            _params_observer.addFuncFileLine( name, func, file, line );
    }

    if(m_argumentsList.contains(name))
    {
        flag = true;
        return m_argumentsList.value(name);
    }
    flag = false;
    return QString();
}

bool CtrConfig::isCmdLineArgumentPresentDef(const QString& name, QString func, QString file, int line)
{
    // push data of function call, if needed
    if( _params_observer_verbose > 0 )
    {
        bool new_item = _params_observer.checkFunctionInMap( name );
        if ( new_item )
            _params_observer.addFuncFileLine( name, func, file, line );
    }

    if(m_argumentsList.contains(name))
        return true;
    return false;
}

QString CtrConfig::getCmdLineFlags()
{
    return m_flagList;
}
