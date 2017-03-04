#include "embeddedsessionmanager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QStringList>
#include <QTimer>

#include "embeddedapp.h"
#include "loadsessionwidget.h"
#include "savesessionwidget.h"
#include "sqliteserializer.h"
#include "version.h"

using namespace ew;

EmbeddedSessionManager::EmbeddedSessionManager(QObject *parent) :
    QObject(parent)
{
    m_pActiveIFace = NULL;
    m_pLazySaveTimer = new QTimer();
    connect(m_pLazySaveTimer, SIGNAL(timeout()), this, SLOT(slotSaveWidgetPrivate()));
    m_pLazySaveTimer->setInterval(1000);
    m_pLazySaveTimer->start();
}


bool EmbeddedSessionManager::checkSerializer()
{
    if(nullptr != m_pActiveIFace)
    {
        if(!m_pActiveIFace->inited())
        {
            qWarning() << __FUNCTION__ << " serializer not inited";
            return false;
        }
    }
    else
    {
        qWarning() << __FUNCTION__ << " serializer not installed";
        return false;
    }

    return true;
}


EmbeddedSessionManager *EmbeddedSessionManager::instance()
{
    static EmbeddedSessionManager self;

    return &self;
}


bool EmbeddedSessionManager::init(ew::SerializerType ctrlr, SerializerConfigBase *cfg, bool * isFirstRun)
{
    bool initRes = false;

    switch(ctrlr)
    {
      case ew::ST_SQLITE:
      {
          SQLiteSerializer *serializPtr = new SQLiteSerializer();
          m_pActiveIFace = serializPtr;
          //SQLiteSerializerConfig* data = dynamic_cast<SQLiteSerializerConfig*>(cfg);
          initRes = m_pActiveIFace->init(cfg, isFirstRun);
      }
      break;
      default:
          break;
    }

    if(checkSerializer())
    {
        QStringList sessions = m_pActiveIFace->getSessions();

        if(!sessions.contains("default"))
        {
            m_pActiveIFace->saveSession("default", QList<EmbeddedSerializationData>(), QMap<quint64, QMap<QString, QVariant> >());
            m_pActiveIFace->attachSession("default");
        }
    }

    return initRes;
}


void EmbeddedSessionManager::slotSaveWidgetPrivate()
{
    QList<quint64> savedList;
    bool check = checkSerializer();

    if(!check)
    {
        return;
    }

    QList<EmbeddedSerializationData> dataList;
    foreach (quint64 id, m_saveList)
    {
        EmbeddedSerializationData data;

        ew::EmbeddedWidgetBaseStructPrivate *priv = ewApp()->getPrivateStruct(id);

        if(nullptr == priv)
        {
            bool res = ewApp()->getLoadedStruct(id, data);

            if(!res)
            {
                qWarning() << "EmbeddedSessionManager::slotSaveWidgetPrivate: WARN! Struct nor found";
                continue;
            }
            else
            {
                dataList.append(data);
            }
        }
        else
        {
            if(priv->embWidgetStruct->widgetTag.isEmpty())
            {
                continue;
            }

            data.data = priv->embWidgetStruct;
            data.id = priv->id;
            data.parent = priv->parentId;
            data.tp = priv->ewType;

            dataList.append(data);
        }
    }


    if(!m_savePropList.isEmpty())
    {
        m_pActiveIFace->saveProperties(m_savePropList);
    }

    if(!dataList.isEmpty())
    {
        m_pActiveIFace->saveWidgets(dataList);
    }

    m_saveList.clear();
    m_savePropList.clear();
}


bool EmbeddedSessionManager::saveProperty(quint64 wId, QString propName, QVariant propValue)
{
    if(!m_savePropList.contains(wId))
    {
        m_savePropList.insert(wId, QMap<QString, QVariant>());
    }

    auto map = m_savePropList.value(wId);
    map.insert(propName, propValue);
    m_savePropList.insert(wId, map);

    return true;
}


bool EmbeddedSessionManager::saveWidget(quint64 widgetId, bool force)
{
    if(!force)
    {
        if(!m_saveList.contains(widgetId))
        {
            m_saveList.append(widgetId);
        }
    }
    else
    {
        m_saveList.append(widgetId);
        slotSaveWidgetPrivate();
    }

    return true;
}


void EmbeddedSessionManager::loadSession(QString sessionName)
{
    if(0 != m_pActiveIFace)
    {
        if(!m_pActiveIFace->inited())
        {
            qWarning() << __FUNCTION__ << " interfaice is not inited";
            return;
        }
    }
    else
    {
        qWarning() << __FUNCTION__ << " serializer is not installed";
        return;
    }

    QMap<quint64, EmbeddedSerializationData> sessionData;
    QMap<quint64, QMap<QString, QVariant> > propMap;

    m_pActiveIFace->loadSession(sessionName, sessionData, propMap);
    emit signalSessionLoaded(sessionName, sessionData, propMap);
}


bool EmbeddedSessionManager::attachSession(QString sessionName)
{
    if(!checkSerializer())
    {
        return false;
    }

    bool res = m_pActiveIFace->attachSession(sessionName);

    return res;
}
