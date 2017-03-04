#ifndef EmbeddedSessionManager_H
#define EmbeddedSessionManager_H

#include <QObject>

#include "embeddedprivate.h"
#include "serializeriface.h"
#include <QStringList>
#include <QVariant>

class QTimer;

namespace ew {
/**
 * @brief Компонент, выполняющий сохранение и загрузку настроек виджетов
 */
class EmbeddedSessionManager : public QObject
{
    Q_OBJECT

private:
    QTimer *m_pLazySaveTimer;
    SerializerIFace *m_pActiveIFace;
    QString m_activeSessionName;
    QList<quint64> m_saveList;
    QMap<quint64, QMap<QString, QVariant> > m_savePropList;

private:
    explicit EmbeddedSessionManager(QObject *parent = 0);
    bool checkSerializer();

public:
    static EmbeddedSessionManager *instance();

    /**
     * @brief Инициализация контроллера.
     * @param ctrlr - тип сериализатора
     * @param cfg - настройки для текущего сериализатора
     * @return true - в случае успешной сериализации
     */
    bool init(ew::SerializerType ctrlr, SerializerConfigBase *cfg, bool *isFirstRun);

    /**
     * @brief Вызывается при необходимости сохранить настройки окна
     * @param widgetId - id окна
     * @return - true в случае постановки задачи на сохранение
     */
    bool saveWidget(quint64 widgetId, bool force = false);

    /**
     * @brief Удаление записи о сохраненном окне. Удаляются все записи дочерних окон.
     * @param widgetId - id окна
     * @return true - в случае успешного выполнения
     */
    bool removeWidget(quint64 widgetId);

    /**
     * @brief Загрузка сохраненной сессий. В результате загрузки формируется сигнал signalSessionLoaded(...)
     *  со списком загруженных сущностей.
     */
    void        loadSession(QString sessionName = "default");
    QStringList getSessionList();


    QString getAttachedSessionName(){return m_activeSessionName; }

    bool detachSession();
    bool attachSession(QString sessionName);
    bool saveSession(QString sessionName, QList<quint64> widgetList);
    bool saveProperty(quint64 wId, QString propName, QVariant propValue);

private slots:
    /**
     * @brief
     */
    void slotSaveWidgetPrivate();

signals:
    /**
     * @brief Сигнал формируется в результате загрузки сессии.
     */
    void signalSessionLoaded(QString sessionName, QMap<quint64, EmbeddedSerializationData> widgetMap, QMap<quint64, QMap<QString, QVariant>> propMap );
};
}

#endif // VISUALIZERCONFIGMANAGER_H
