#include "ctrcore.h"
#include "ctrconfig.h"
#include "ctrdefines.h"

#include <unistd.h>
#include <getopt.h>
#include <stdio.h>

//#include <objrepr/entity.h>
//#include <objrepr/reprServer.h>

#include <QDir>
#include <stdio.h>

#include <QApplication>
#include <QMessageBox>
#include <QtGlobal>

#include <QMessageBox>
#include <QFileInfo>
#include <dmanager/embeddedapp.h>
#include <dmanager/sqliteserializer.h>
#include <dmanager/docviewerwidget.h>
//using namespace unilog;
using namespace ctrcore;

CtrCore::CtrCore(QObject *parent) :
    QObject(parent)
{
    translator = new QTranslator();
    if(translator->load(":/ctrcore.qm"))
    {
       QApplication::installTranslator(translator);
    }
    translators.append(translator);
    translator = new QTranslator();
    if(translator->load(":/qt_ru.qm"))
    {
        QApplication::installTranslator(translator);
    }
    translators.append(translator);

    //ewApp()->warning(0,tr("contour ng"),"msg",QMessageBox::Close,QMessageBox::Close);
}

void CtrCore::qtMessageHandler(QtMsgType msgType, const char *msg)
{
//    switch (msgType) {
//    case QtDebugMsg:
//        UNILOG_DEBUG_DEF(QT) << msg << std::endl;
//        break;
//    case QtWarningMsg:
//        UNILOG_WARNING_DEF(QT) << msg << std::endl;
//        break;
//    case QtCriticalMsg:
//        UNILOG_ERROR_DEF(QT) << msg << std::endl;
//        break;
//    case QtFatalMsg:
//        UNILOG_CRIT_DEF(QT) << msg << std::endl;
//        break;
//    }
}

CtrCore::~CtrCore()
{
    while(!translators.isEmpty())
    {
        delete translators.takeFirst();
    }
    delete CtrConfig::getConfig();
//    UNILOG_DEBUG (_logger) << "Destroy CtrCore" << std::endl;
}

bool CtrCore::init()
{
    CtrConfig::parseCmdLine();

    bool res = initConfigs() && initEmbeddedLib();

//    bool res = initLogger();
//    res = res  && initConfigs();
//    res = res  && initEmbeddedLib();
//    res = res  && initRepresentationServer();

    return res;
}

bool CtrCore::initLogger()
{
//    bool fl(false);
//    if(CtrConfig::isCmdLineArgumentPresent(ARG_UNILOG_CONFIG))
//    {
//        QString path = CtrConfig::getCmdLineValue(ARG_UNILOG_CONFIG, fl);
//        fl = unilog::Unilog::instance()->configure(path.toUtf8().data());
//    }
//    else if (QFile::exists(CtrConfig::getUserConfigPath().append("/unilog.conf")))
//    {
//        fl = unilog::Unilog::instance()->configure(CtrConfig::getUserConfigPath().append("/unilog.conf").toStdString().data());
//    }
//    else
//    {
//        // инициализавция пути к логгеру из конфиг файла убрана,
//        // логгер проинициализирован по умолчанию
//        fl = unilog::Unilog::instance()->configure();
//    }

//    if(!fl)
//    {
//        QMessageBox::critical(NULL, tr("Critical error"), unilog::Unilog::instance()->errString());
//        return false;
//    }
//    else
//    {
//        _logger = unilog::Unilog::instance()->getLogger(unilog::Unilog::FC_CORE, CTRCORE);
//        UNILOG_INFO(_logger) << "Logger initialized by:"
//                             << unilog::Unilog::instance()->configFilePath()
//                             << std::endl;
//        _qtLogger = unilog::Unilog::instance()->getLogger(unilog::Unilog::FC_CORE, CTRCORE);
//        UNILOG_INFO(_qtLogger) << "QDebug logger initialized" << std::endl;
//        qInstallMsgHandler(qtMessageHandler);
//    }
    return true;
}


bool CtrCore::initConfigs()
{
    return CtrConfig::checkConfigs();
}

bool CtrCore::initRepresentationServer()
{
//    QString configPath;

//    bool fl(true);
//    bool fileExist(true);
//    bool objreprFilePathExist(true);
//    QString commonPath = ctrconfig::getCommonConfigPath(CtrConfig::getAppName())+"/";

//    QFileInfo finf;
//    finf.setFile(commonPath+QString(CTR_SYSTEM_FILE_NAME)+".xml");
//    fileExist = finf.exists();
//    /// Проверка наличия ctrsystem.xml в /etc/<app_name>/
//    if(!fileExist)
//    {
//        showErrorMessage(commonPath+QString(CTR_SYSTEM_FILE_NAME)+".xml "+tr("Core config files not found!"));
//        /// Не найдены конфигурационные файлы комплекса!
//        return false;
//    }

//    configPath = CtrConfig::getValueByName(QString(CTR_SYSTEM_FILE_NAME).append(".settings").append(".objrepr")).toString();
//    /// Проверка наличия тэга <objrepr> в файле ctrsystem.xml
//    if(configPath.isEmpty())
//    {
//        showErrorMessage(tr("The Representation server config files not found!"));
//        ///Не найден конфигурационный файл сервера модели данных (config.xml)!
//        return false;
//    }

//    if(CtrConfig::isCmdLineArgumentPresent(ARG_OBJREPR_CONFIG))
//        configPath = CtrConfig::getCmdLineValue(ARG_OBJREPR_CONFIG, fl);
//    else
//        configPath =
//                CtrConfig::getValueByName(QString(CTR_SYSTEM_FILE_NAME).append(".settings").append(".objrepr"),CtrConfig::getUserConfigPath().append("/config.xml"), true).toString();

//    finf.setFile(configPath);
//    objreprFilePathExist = finf.exists();
//    /// Проверка наличия файла по адресу указанному в теге <objrepr> в файле ctrsystem.xml
//    if(!objreprFilePathExist)
//    {
//        showErrorMessage(tr("The Representation server config files not found!"));
//        ///Не найден конфигурационный файл сервера модели данных (config.xml)!
//        return false;
//    }
    
//    UNILOG_INFO(_logger) << "ObjReper config path is: " << configPath.toStdString().data() << std::endl;

//    bool result = objrepr::RepresentationServer::instance()->configure(configPath.toStdString().data());
//    if (!result)
//    {
//        UNILOG_CRIT(_logger) <<  "Representation server configuring error:"
//                              << objrepr::RepresentationServer::instance()->errString()
//                              << std::endl;
//    }
//    else
//    {
//        result = objrepr::RepresentationServer::instance()->launch();

//        if (!result)
//        {
//            UNILOG_CRIT(_logger) << "Representation server launching error: "
//                                 << objrepr::RepresentationServer::instance()->errString()
//                                 << std::endl;
//        }
//        else
//        {
//            UNILOG_INFO(_logger) << "Representation server launched" << std::endl;
//            return true;
//        }
//    }

    return false;
}

bool CtrCore::initEmbeddedLib()
{
    //// Embedded initialization
    SQLiteSerializerConfig * conf = new SQLiteSerializerConfig(0);
    conf->configDir = CtrConfig::getUserConfigPath();

    QString appName = CtrConfig::getAppName();
    QVariant docDirPath = CtrConfig::getValueByName(QString("embeddedwidgets.docDirPath"));
    if(!docDirPath.isValid())
    {
        QString appDocDir = QString(QDir::separator()) + "opt" + QDir::separator() + appName + QDir::separator() + "doc";
        CtrConfig::setValueByName(QString("embeddedwidgets.docDirPath"), appDocDir);
        docDirPath = CtrConfig::getValueByName(QString("embeddedwidgets.docDirPath"));
    }

    QVariant docPort = CtrConfig::getValueByName(QString("embeddedwidgets.docDebugPort"));
    if(!docPort.isValid())
    {
        CtrConfig::setValueByName(QString("embeddedwidgets.docDebugPort"), 20342);
        docPort = CtrConfig::getValueByName(QString("embeddedwidgets.docDebugPort"));
    }

    const QDir configDir(conf->configDir);
    const QString databaseName = "embedded.sqlite"; // Файл БД EmbededdWidgets. Для проверки первого запуска.
    const QString stylesheetName = "artStyle.qss"; // Имя стиля по умолчанию
    // Установка стиля по умолчанию
    bool isFirstRun = !configDir.exists(databaseName);
    ewApp()->init(ew::ST_SQLITE,conf,ew::network::EmbeddedNetworkManager::WM_SERVER,docPort.toInt(),docDirPath.toString());


    if (isFirstRun)
    {
        QStringList stylesheetsList = ewApp()->getAvailableStylesheets();

        if (stylesheetsList.contains(stylesheetName))
        {
            ewApp()->setStylesheet(stylesheetName);
        }
    }

    ewApp()->loadLastSession();

    return true;
}

void CtrCore::showErrorMessage(const QString &msg)
{
    ewApp()->information(0,tr("contour ng"),msg,QMessageBox::Close,QMessageBox::Close);
}

