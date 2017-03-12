#include "sqliteserializer.h"

#include <QCoreApplication>

#include "embeddedstruct.h"
#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlRecord>
#include <QTime>
#include <QVariant>


SQLiteSerializerConfig::SQLiteSerializerConfig(QObject *parent) :
    SerializerConfigBase(parent)
{
    configDir = "./configs";
}


SQLiteSerializer::SQLiteSerializer(QObject *parent) :
    SerializerIFace(parent),
    m_dbFileName("embedded.sqlite"),
    m_dbVersion(101)
{
    m_inited = false;
    m_attachedSession = "default";
}


bool SQLiteSerializer::init(SerializerConfigBase *cfg, bool *isFirstRun)
{
    SQLiteSerializerConfig *sqlConf = dynamic_cast<SQLiteSerializerConfig *>(cfg);

    if(nullptr == sqlConf)
    {
        qDebug() << __FUNCTION__ << " Incomplete initialization";
        return false;
    }

    QString settingsDirPath = sqlConf->configDir;
    bool res = false;
    QDir dr(settingsDirPath);

    if(dr.exists())
    {
        res = initDatabase(settingsDirPath, isFirstRun);

        if(res)
        {
            clearProcedure();
        }
        else
        {
            qDebug() <<  "SQLiteSerializer::init(): init error ";
        }
    }
    else
    {
        qDebug() << "SQLiteSerializer::init(): init dir not exist " << settingsDirPath;
    }

    return res;
}


void SQLiteSerializer::saveWidget(EmbeddedSerializationData emb)
{
    if(!inited())
    {
        qWarning() << "SQLiteSerializer::saveWidget(): database not inited";
        return;
    }


    QString request = QString ("INSERT OR REPLACE INTO widgets ( id, tag, parent, align, x, y, w, h, type, visible, session ) \
                               VALUES ( :id, :tag, :parent, :align, :x, :y, :w, :h, :type, :visible, :session );");

    /*bool res =  */ _query.prepare(request);
    _query.bindValue(QString(":id"), emb.id);
    _query.bindValue(QString(":tag"), emb.data->widgetTag);
    _query.bindValue(QString(":parent"), emb.parent);
    _query.bindValue(QString(":align"), (int)emb.data->alignType);

    _query.bindValue(QString(":x"), emb.data->alignPoint.x());
    _query.bindValue(QString(":y"), emb.data->alignPoint.y());
    _query.bindValue(QString(":w"), emb.data->size.width());
    _query.bindValue(QString(":h"), emb.data->size.height());
    _query.bindValue(QString(":type"), emb.tp);
    _query.bindValue(QString(":visible"), !emb.data->addHided);
    _query.bindValue(QString(":session"), m_attachedSession);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::saveWidget(...): can't save widget!";
        qDebug() << _query.lastError().text();
        qDebug() << _query.lastQuery();
    }

    request = QString ("INSERT OR REPLACE INTO widget_settings ( id, allow_move, allow_change_parent, locked, collapsed, min_w, min_h, max_w, max_h, \
                       modal, block_modal, header_vis, stylesheet, size_policy, session, top_on_hint, allow_insert, maximized, auto_adjust) \
              VALUES ( :id, :allow_move, :allow_change_parent, :locked, :collapsed, :min_w, :min_h , :max_w, :max_h, \
                       :modal, :block_modal, :header_vis, :stylesheet, :size_policy, :session, :top_on_hint, :allow_insert, :maximized, :auto_adjust );");

    /*res =  */ _query.prepare(request);
    _query.bindValue(QString(":id"), emb.id);
    _query.bindValue(QString(":allow_move"), emb.data->allowMoving);
    _query.bindValue(QString(":allow_change_parent"), emb.data->allowChangeParent);
    _query.bindValue(QString(":locked"), emb.data->locked);
    _query.bindValue(QString(":collapsed"), emb.data->collapsed);
    _query.bindValue(QString(":min_w"), emb.data->minSize.width());
    _query.bindValue(QString(":min_h"), emb.data->minSize.height());
    _query.bindValue(QString(":max_w"), emb.data->maxSize.width());
    _query.bindValue(QString(":max_h"), emb.data->maxSize.height());
    _query.bindValue(QString(":modal"), emb.data->isModal);
    _query.bindValue(QString(":block_modal"), emb.data->isModalBlock);
    _query.bindValue(QString(":header_vis"), emb.data->headerVisible);
    _query.bindValue(QString(":size_policy"), (int)emb.data->szPolicy);
    _query.bindValue(QString(":stylesheet"), emb.data->stylesheetFile);
    _query.bindValue(QString(":size_policy"), (int)emb.data->szPolicy);
    _query.bindValue(QString(":session"), m_attachedSession);
    _query.bindValue(QString(":top_on_hint"), emb.data->topOnHint);
    _query.bindValue(QString(":allow_insert"), emb.data->allowGrouping);
    _query.bindValue(QString(":maximized"), emb.data->maximized);
    _query.bindValue(QString(":auto_adjust"), emb.data->autoAdjustSize);



    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::saveWidget(...): can't save widget_settings!";
        qDebug() << _query.lastError().text();
        qDebug() << _query.lastQuery();
    }

    request = QString ("INSERT OR REPLACE INTO widget_header ( id, close_btn, global_move_btn, collapse_btn, pin_btn, what_btn, lock_btn, min_max_btn, settings_btn \
                       , style_btn, midget_btn, hide_header_btn, descr, title, tooltip, short_text, header_align, session, icon) \
              VALUES ( :id, :close_btn, :global_move_btn, :collapse_btn, :pin_btn, :what_btn, :lock_btn, :min_max_btn, :settings_btn \
                       , :style_btn, :midget_btn, :hide_header_btn, :descr, :title, :tooltip, :short_text, :header_align, :session, :icon) \;");

    /*res =  */ _query.prepare(request);
    ew::EmbeddedHeaderStruct & head = emb.data->header;
    _query.bindValue(QString(":id"), emb.id);
    _query.bindValue(QString(":close_btn"), head.hasCloseButton);
    _query.bindValue(QString(":global_move_btn"), head.hasGlobalMoveButton);
    _query.bindValue(QString(":collapse_btn"), head.hasCollapseButton);
    _query.bindValue(QString(":pin_btn"), head.hasPinButton);
    _query.bindValue(QString(":what_btn"), head.hasWhatButton);
    _query.bindValue(QString(":lock_btn"), head.hasLockButton);
    _query.bindValue(QString(":min_max_btn"), head.hasMinMaxButton);
    _query.bindValue(QString(":style_btn"), head.hasStyleButton);
    _query.bindValue(QString(":midget_btn"), head.hasMidgetButton);
    _query.bindValue(QString(":hide_header_btn"), head.hasHideHeaderButton);
    _query.bindValue(QString(":descr"), head.descr);
    _query.bindValue(QString(":title"), head.windowTitle);
    _query.bindValue(QString(":tooltip"), head.tooltip);
    _query.bindValue(QString(":short_text"), head.visibleCbText);
    _query.bindValue(QString(":header_align"), head.align);
    _query.bindValue(QString(":session"), m_attachedSession);
    _query.bindValue(QString(":settings_btn"), head.hasSettingsButton);
    _query.bindValue(QString(":icon"), head.headerPixmap);


    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::saveWidget(...): can't save widget_header!";
        qDebug() << _query.lastError().text();
        qDebug() << _query.lastQuery();
    }
}


void SQLiteSerializer::saveProperties(QMap<quint64, QMap<QString, QVariant> > propMap)
{
    QMapIterator<quint64, QMap<QString, QVariant> > varIter(propMap);

    while(varIter.hasNext())
    {
        varIter.next();
        QMapIterator<QString, QVariant> propIter(varIter.value());

        while(propIter.hasNext())
        {
            propIter.next();
            saveProperty(varIter.key(), propIter.key(), propIter.value());
        }
    }
}


void SQLiteSerializer::saveProperty(quint64 wId, QString propName, QVariant propValue)
{
    QString request = QString ("INSERT OR REPLACE INTO widget_metainfo ( id, param_name, param_type, param_value, session) \
                               VALUES ( :id, :param_name, :param_type, :param_value, :session );");
    int propType = (int)propValue.type();

    /*bool res =  */ _query.prepare(request);
    _query.bindValue(QString(":id"), wId);
    _query.bindValue(QString(":param_name"), propName);
    _query.bindValue(QString(":param_type"), propType);
    _query.bindValue(QString(":param_value"), propValue);
    _query.bindValue(QString(":session"), m_attachedSession);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::saveWidget(...): can't save property!";
        qDebug() << _query.lastError().text();
        qDebug() << _query.lastQuery();
    }
}


SQLiteSerializer::~SQLiteSerializer()
{
    _db.commit();
    clearProcedure();
}


bool SQLiteSerializer::inited()
{
    return m_inited;
}


void SQLiteSerializer::clearProcedure()
{
    if(!inited())
    {
        qWarning() << "SQLiteSerializer::clearProcedure(): database not inited";
        return;
    }

    qDebug() << "SQLiteSerializer::clearProcedure()";

    QString request;
    request = QString("DELETE FROM widgets WHERE NOT EXISTS(SELECT name FROM sessions ) ;");
    _query.prepare(request);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::clearProcedure(...): Can't delete widgets";
        qDebug() << _query.lastError().text();

        qDebug() << _query.lastQuery();
        return;
    }

    request = QString("DELETE FROM widget_settings WHERE NOT EXISTS(SELECT name FROM sessions ) ;");
    _query.prepare(request);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::clearProcedure(...): Can't delete widgets";
        qDebug() << _query.lastError().text();

        qDebug() << _query.lastQuery();
        return;
    }

    request = QString("DELETE FROM widget_header WHERE NOT EXISTS(SELECT name FROM sessions ) ;");
    _query.prepare(request);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::clearProcedure(...): Can't delete widgets";
        qDebug() << _query.lastError().text();

        qDebug() << _query.lastQuery();
        return;
    }

    request = QString("DELETE FROM widget_metainfo WHERE NOT EXISTS(SELECT name FROM sessions ) ;");
    _query.prepare(request);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::clearProcedure(...): Can't delete widgets";
        qDebug() << _query.lastError().text();

        qDebug() << _query.lastQuery();
        return;
    }
}


//void SQLiteSerializer::onRemoveVisualizer(vismantypes::VisualizerInfo inf)
//{

//    quint64 globVisId = localVisIdToGlobal(inf.localVisId);

//    if(-1 == globVisId)
//        return ;

//    QString request;
//    request = QString("DELETE FROM plugininfo WHERE NOT EXISTS(SELECT vis_id FROM sessions) AND vis_id = ");
//    request.append(QString::number(globVisId) + " ;");
//    _query.prepare(request);

//    if (!_query.exec())
//    {
//        qDebug() << "SQLiteSerializer::clearProcedure(...): Can't delete plugins";
//        qDebug() << _query.lastError().text();
//        qDebug() << _query.lastQuery();
//        return;
//    }



//    request = QString("DELETE FROM visualizers WHERE NOT EXISTS(SELECT vis_id FROM sessions) AND vis_id = ");
//    request.append(QString::number(globVisId) + " ;");
//    _query.prepare(request);

//    if (!_query.exec())
//    {
//        qDebug() << "SQLiteSerializer::clearProcedure(...): can't delete visualizers";
//        qDebug() << _query.lastError().text();
//        qDebug() << _query.lastQuery();
//        return;
//    }

//    m_globalToLocalIds.remove(globVisId);


//}

bool SQLiteSerializer::initDatabase(QString dbDirPath, bool *isFirstRun)
{
    if(m_inited)
    {
        qDebug() << "SQLiteSerializer::initDatabase(): already inited!";
        return false;
    }

    QString fullFileName = dbDirPath + QDir::separator() + m_dbFileName;
    QFileInfo fi(fullFileName);
    qDebug() << "SQLiteSerializer::initDatabase() path: " << fi.absolutePath();

    if(QFile(fullFileName).exists())
    {
        qDebug () << "SQLiteSerializer::initDatabase: Avaliable drivers: " <<  QSqlDatabase::drivers();

        _db = QSqlDatabase::addDatabase("QSQLITE", "EMBEDED");
        _db.setDatabaseName(fullFileName);

        if(!_db.open())
        {
            qDebug() << "SQLiteSerializer::initDatabase: Error load db!" << _db.lastError().text();
        }
        else
        {
            _query = QSqlQuery(_db);
        }

        int ver = 0;
        QString query = "PRAGMA schema_version;";
        _query.prepare(query);

        if(!_query.exec())
        {
            qDebug() << "SQLiteSerializer::init(...): ";
            qDebug() << _query.lastError().text();
            qDebug() << _query.lastQuery();
        }
        else
        {
            while(_query.next())
            {
                ver = _query.value(0).toInt();
            }
        }

        if(m_dbVersion != ver )
        {
            qDebug() << "SQLiteSerializer::init(...): old database detected. Reinit";

            QFile f(":/embwidgets/db/db/" + m_dbFileName);

            if(!f.open(QFile::ReadOnly))
            {
                qDebug() << "SQLiteSerializer::initDatabase: Can't read config database from resources";
                m_inited = false;
                return false;
            }

            bool res = QFile::remove(fullFileName);

            if(!res)
            {
                qDebug() << "SQLiteSerializer::initDatabase: Can't delete old database!";
                return false;
            }

            QByteArray arr = f.readAll();
            QFile createdDb(fullFileName);

            if(!createdDb.open(QFile::WriteOnly))
            {
                qDebug() << "SQLiteSerializer::initDatabase: Can't create config database";
                m_inited = false;
                return false;
            }

            createdDb.write(arr);
            createdDb.close();

            _db.removeDatabase(fullFileName);
            _db.setDatabaseName(fullFileName);

            if(!_db.open())
            {
                qDebug() << "SQLiteSerializer::initDatabase: Error load visualizers config!" << _db.lastError().text();
            }
            else
            {
                _query = QSqlQuery(_db);

                QString query = "PRAGMA schema_version;";
                _query.prepare(query);

                if(!_query.exec())
                {
                    qDebug() << "SQLiteSerializer::init(...): ";
                    qDebug() << _query.lastError().text();
                    qDebug() << _query.lastQuery();
                }
                else
                {
                    while(_query.next())
                    {
                        ver = _query.value(0).toInt();
                    }

                    if(ver != m_dbVersion)
                    {
                        qDebug() << "SQLiteSerializer::init(...): FATAL! DATABASE INCOMPLETE. DB and code versions mismatch";
                    }
                }
            }
        }
    }
    else
    {
        *isFirstRun = true;
        QFile f(":/embwidgets/db/db/" + m_dbFileName);

        if(!f.open(QFile::ReadOnly))
        {
            qDebug() << "SQLiteSerializer::initDatabase: Can't read config database from resources";
            m_inited = false;
            return false;
        }

        QByteArray arr = f.readAll();
        QFile createdDb(fullFileName);

        if(!createdDb.open(QFile::WriteOnly))
        {
            qDebug() << "SQLiteSerializer::initDatabase: Can't create config database";
            m_inited = false;
            return false;
        }

        createdDb.write(arr);
        createdDb.close();

        _db = QSqlDatabase::addDatabase("QSQLITE");
        _db.setDatabaseName(fullFileName);

        if(!_db.open())
        {
            qDebug() << "SQLiteSerializer::initDatabase: Error load visualizers config!" << _db.lastError().text();
        }
        else
        {
            _query = QSqlQuery(_db);

            int ver = 0;
            QString query = "PRAGMA schema_version;";
            _query.prepare(query);

            if(!_query.exec())
            {
                qDebug() << "SQLiteSerializer::init(...): ";
                qDebug() << _query.lastError().text();
                qDebug() << _query.lastQuery();
            }
            else
            {
                while(_query.next())
                {
                    ver = _query.value(0).toInt();
                }

                if(ver != m_dbVersion)
                {
                    qDebug() << "SQLiteSerializer::init(...): FATAL! DATABASE INCOMPLETE. DB and code versions mismatch";
                }
            }
        }
    }

    _db.exec("PRAGMA synchronous = NORMAL");
    _db.exec("PRAGMA journal_mode = WAL");



    QString query = "SELECT id from widgets ;";
    _query.prepare(query);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::init(...): Can't load widgets ids ids";
        qDebug() << _query.lastError().text();
        qDebug() << _query.lastQuery();
    }

    m_inited = true;
    return true;
}


void SQLiteSerializer::removeSession(QString sessionName)
{
    QString request = QString("DELETE FROM sessions WHERE name = \'");

    request.append(sessionName + "\' ;");

    /*bool res =  */ _query.prepare(request);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::removeSession(...): Can't remove session!";
        qDebug() << _query.lastError().text();
        qDebug() << _query.lastQuery();
        return;
    }
}


QStringList SQLiteSerializer::getFreeSessions()
{
    QStringList lst;
    QString request("SELECT (name) FROM sessions WHERE locked == 0");

    _query.prepare(request);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::getFreeSessions(...): ";
        qDebug() << _query.lastError().text();
        qDebug() << _query.lastQuery();
    }
    else
    {
        while(_query.next())
        {
            lst.append(_query.value(0).toString());
        }
    }

    return lst;
}


void SQLiteSerializer::removeWidget(quint64 widgetId)
{
    QString request;

    request = QString("DELETE FROM widgets WHERE SESSION=\'") + m_attachedSession + "\'"
              + " AND id=" + QString::number(widgetId);
    _query.prepare(request);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::removeWidget(...): Can't delete widgets";
        qDebug() << _query.lastError().text();

        qDebug() << _query.lastQuery();
        return;
    }
}


void SQLiteSerializer::removeWidgets(QList<quint64> idList)
{
    foreach (quint64 id, idList)
    {
        removeWidget(id);
    }

    _db.commit();
}


void SQLiteSerializer::saveWidgets(QList<EmbeddedSerializationData> embList)
{
    foreach (EmbeddedSerializationData dta, embList)
    {
        saveWidget(dta);
    }

    bool res = _db.commit();
//    if(!res)
//    {
//        qDebug() << "SQLiteSerializer::saveWidgets: ERROR! Commit failed!";
//        qDebug() << _db.lastError().text();
//    }
}


void SQLiteSerializer::saveSession(QString sessionName, QList<EmbeddedSerializationData> strList, QMap<quint64, QMap<QString, QVariant> > propMap)
{
    removeSession(sessionName);

    QString request("INSERT INTO sessions (name, locked) VALUES ( \'");
    request.append( sessionName + "\', ");
    request.append( QString::number(0) + ") ;");
    _query.prepare(request);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::saveSession(...)";
        qDebug() << _query.lastError().text();
        qDebug() << _query.lastQuery();
    }

    saveWidgets(strList);
    saveProperties(propMap);
}


bool SQLiteSerializer::loadSession(const QString sessionName
                                   , QMap<quint64, EmbeddedSerializationData> & widgList,
                                   QMap<quint64, QMap<QString, QVariant> > & propMap)
{
    QMap<quint64, EmbeddedSerializationData> lst;

    QString request("SELECT * FROM widgets WHERE session == \'");
    request.append(sessionName);
    request.append("\';");

    _query.prepare(request);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::loadSession(...): ";
        qDebug() << _query.lastError().text();
        qDebug() << _query.lastQuery();
        return false;
    }
    else
    {
        while(_query.next())
        {
            QSqlRecord record = _query.record();
            EmbeddedSerializationData val;
            ew::EmbeddedWidgetBaseStruct *str = 0;


            val.tp  = (ew::EmbeddedWidgetType)_query.value(record.indexOf("type")).toInt();

            switch(val.tp)
            {
              case ew::EWT_GROUP:
              {
                  str = new ew::EmbeddedGroupStruct();
              } break;
              case ew::EWT_WINDOW:
              {
                  str = new ew::EmbeddedWindowStruct();
              } break;
              case ew::EWT_WIDGET:
              {
                  str = new ew::EmbeddedWidgetStruct();
              } break;
              case ew::EWT_SERVICE_WIDGET:
              {
                  str = new ew::EmbeddedWidgetStruct();
              } break;
              case ew::EWT_MAINWINDOW:
              {
                  str = new ew::EmbeddedMainWindowStruct();
              } break;
              default:
                  break;
            }

            val.data = str;

            val.id = _query.value(record.indexOf("id")).toULongLong();
            str->widgetTag = _query.value(record.indexOf("tag")).toString();
            val.parent = _query.value(record.indexOf("parent")).toULongLong();
            str->alignType = (ew::EmbeddedWidgetAlign)_query.value(record.indexOf("align")).toInt();
            str->alignPoint = QPoint(_query.value(record.indexOf("x")).toDouble(),
                                     _query.value(record.indexOf("y")).toDouble());
            str->size = QSize(_query.value(record.indexOf("w")).toInt(),
                              _query.value(record.indexOf("h")).toInt());
            str->addHided = !_query.value(record.indexOf("visible")).toBool();

            if(ew::EWT_MAINWINDOW == val.tp)
            {
            }

            lst.insert(val.id, val);

            if(ew::EWT_MAINWINDOW == val.tp)
            {
            }
        }
    }

    request = QString ("SELECT * FROM widget_header WHERE session == \'");
    request.append(sessionName);
    request.append("\';");

    _query.prepare(request);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::loadSession(...): ";
        qDebug() << _query.lastError().text();
        qDebug() << _query.lastQuery();

        return false;
    }
    else
    {
        while(_query.next())
        {
            QSqlRecord record = _query.record();
            quint64 id = _query.value(record.indexOf("id")).toULongLong();

            if(!lst.contains(id))
            {
                qWarning() << "SQLiteSerializer::loadSession(...): incomplete data in widget_header";
                continue;
            }

            EmbeddedSerializationData val = lst.value(id);
            ew::EmbeddedWidgetBaseStruct *str = val.data;

            str->header.hasCloseButton = _query.value(record.indexOf("close_btn")).toBool();
            str->header.hasGlobalMoveButton = _query.value(record.indexOf("global_move_btn")).toBool();
            str->header.hasCollapseButton = _query.value(record.indexOf("collapse_btn")).toBool();
            str->header.hasPinButton = _query.value(record.indexOf("pin_btn")).toBool();
            str->header.hasWhatButton = _query.value(record.indexOf("what_btn")).toBool();
            str->header.hasLockButton = _query.value(record.indexOf("lock_btn")).toBool();
            str->header.hasMinMaxButton = _query.value(record.indexOf("min_max_btn")).toBool();
            str->header.hasSettingsButton = _query.value(record.indexOf("settings_btn")).toBool();
            str->header.hasStyleButton = _query.value(record.indexOf("style_btn")).toBool();
            str->header.hasMidgetButton = _query.value(record.indexOf("midget_btn")).toBool();
            str->header.hasHideHeaderButton = _query.value(record.indexOf("hide_header_btn")).toBool();
            str->header.descr = _query.value(record.indexOf("descr")).toString();
            str->header.windowTitle = _query.value(record.indexOf("title")).toString();
            str->header.tooltip = _query.value(record.indexOf("tooltip")).toString();
            str->header.visibleCbText = _query.value(record.indexOf("short_text")).toString();
            str->header.align = (ew::EmbeddedHeaderAlign)_query.value(record.indexOf("header_align")).toInt();
            str->header.headerPixmap = _query.value(record.indexOf("icon")).toString();

            lst.insert(id, val);
        }
    }

    request = QString ("SELECT * FROM widget_settings WHERE session == \'");
    request.append(sessionName);
    request.append("\';");

    _query.prepare(request);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::loadSession(...): ";
        qDebug() << _query.lastError().text();
        qDebug() << _query.lastQuery();
        return false;
    }
    else
    {
        while(_query.next())
        {
            QSqlRecord record = _query.record();
            quint64 id = _query.value(record.indexOf("id")).toULongLong();

            if(!lst.contains(id))
            {
                qWarning() << "SQLiteSerializer::loadSession(...): incomplete data in widget_header";
                continue;
            }

            EmbeddedSerializationData val = lst.value(id);
            ew::EmbeddedWidgetBaseStruct *str = val.data;

            str->allowMoving = _query.value(record.indexOf("allow_move")).toBool();
            str->allowChangeParent = _query.value(record.indexOf("allow_change_parent")).toBool();
            str->locked = _query.value(record.indexOf("locked")).toBool();
            str->collapsed = _query.value(record.indexOf("collapsed")).toBool();

            str->minSize = QSize(_query.value(record.indexOf("min_w")).toInt(),
                                 _query.value(record.indexOf("min_h")).toInt());

            str->maxSize = QSize(_query.value(record.indexOf("max_w")).toInt(),
                                 _query.value(record.indexOf("max_h")).toInt());

            str->isModal = _query.value(record.indexOf("modal")).toBool();
            str->isModalBlock = _query.value(record.indexOf("block_modal")).toBool();

            str->headerVisible = _query.value(record.indexOf("header_vis")).toBool();
            str->szPolicy = (ew::EmbeddedSizePolicy)_query.value(record.indexOf("size_policy")).toInt();
            str->stylesheetFile = _query.value(record.indexOf("stylesheet")).toString();

            str->stylesheetFile = _query.value(record.indexOf("stylesheet")).toString();
            str->topOnHint = _query.value(record.indexOf("top_on_hint")).toBool();
            str->allowGrouping = _query.value(record.indexOf("allow_insert")).toBool();
            str->maximized = _query.value(record.indexOf("maximized")).toBool();
            str->autoAdjustSize = _query.value(record.indexOf("auto_adjust")).toBool();

            lst.insert(id, val);
        }
    }

    request = QString ("SELECT * FROM widget_metainfo WHERE session == \'");
    request.append(sessionName);
    request.append("\';");

    _query.prepare(request);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::loadSession(...): ";
        qDebug() << _query.lastError().text();
        qDebug() << _query.lastQuery();
        return false;
    }
    else
    {
        while(_query.next())
        {
            QSqlRecord record = _query.record();
            quint64 id = _query.value(record.indexOf("id")).toULongLong();

            if(!lst.contains(id))
            {
                qWarning() << "SQLiteSerializer::loadSession(...): incomplete data in widget_header";
                continue;
            }

            //EmbeddedSerializationData val = lst.value(id);
            //ew::EmbeddedWidgetBaseStruct *str = val.data;

            QString name = _query.value(record.indexOf("param_name")).toString();
            QVariant::Type tp = (QVariant::Type )_query.value(record.indexOf("param_type")).toInt();
            QVariant value = _query.value(record.indexOf("param_value"));
            value.convert(tp);

            if(!propMap.contains(id))
            {
                propMap.insert(id, QMap<QString, QVariant>());
            }

            auto mp = propMap.value(id);
            mp.insert(name, value);
            propMap.insert(id, mp);
        }
    }

    widgList = lst;


    return true;
}


QStringList SQLiteSerializer::getSessions()
{
    QStringList resList;
    QString request("SELECT * FROM sessions ;");

    _query.prepare(request);

    if(!_query.exec())
    {
        qDebug() << "SQLiteSerializer::getSessions(...): ";
        qDebug() << _query.lastError().text();
        qDebug() << _query.lastQuery();
        return resList;
    }
    else
    {
        while(_query.next())
        {
            QSqlRecord record = _query.record();
            resList.append(_query.value(record.indexOf("name")).toString());
        }
    }

    return resList;
}


QString SQLiteSerializer::alignTypeToString(ew::EmbeddedWidgetAlign aType)
{
    return QString::number((int)aType);
}


ew::EmbeddedWidgetAlign SQLiteSerializer::alignTypeFromString(QString str)
{
    bool ok = true;

    ew::EmbeddedWidgetAlign aType = (ew::EmbeddedWidgetAlign)(str.toInt(&ok, 10));

    return aType;
}


QString SQLiteSerializer::widgetTypeToString(ew::EmbeddedWidgetType wType)
{
    return QString::number((int)wType);
}


ew::EmbeddedWidgetType SQLiteSerializer::widgetTypeFromString(QString str)
{
    bool ok = true;

    ew::EmbeddedWidgetType wt = (ew::EmbeddedWidgetType)str.toInt(&ok);
    return wt;
}
