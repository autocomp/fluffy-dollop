#ifndef APPCOMPOSER_H
#define APPCOMPOSER_H

#include <QObject>
#include <QVariantMap>
#include <dmanager/embeddedstruct.h>
#include <dmanager/embeddedsubiface.h>
#include <ctrcore/bus/globalmessages.h>
#include <ctrcore/plugin/ctrappvisualizerplugin.h>

class QAction;
class ScenarioSelector;
class ManagerVectorObjectPlugin;
class QMenuBar;
class GlobalMessageNotifier;
class QMenu;
class EndWorkingObject;

/**
 * @brief Главный невизуальный класс приложения, реализующий инициализацию, загрузку и компоновку.
 */
class AppComposer : public QObject
{
    Q_OBJECT

public:
    explicit AppComposer(QObject *parent = 0);
    ~AppComposer();
    void init();
    QWidget* createBottomBar();
    QMenuBar* createMenuBar();


signals:

protected slots:
    /**
     * @brief Глобальные сообщения
     * @param mt - тип сообщения
     * @param var - данные сообщения
     */
    void slotGlobalMessageReceived(globalbusmessages::GLOBAL_MESSAGE_TYPE mt,QVariant var);
    /**
     * @brief Срабатывает при нажатии кнопки главного меню приложения
     */
    void slotMainMenuClicked();

    /**
     * @brief Срабатывает при нажатии кнопки главного меню приложения
     */
    void slotStyleClicked(bool);

    /**
     * @brief Слот срабатывает при изменении мета-информации у встроенного виджета. Изменение является внутренним для виджета.
     */
    void slotWidgetMetaInfoChanged();

    void slotSavedWidgetLoaded(QString widgetTag, QVariantMap widgetMetaInfo, ew::EmbeddedWidgetType widgetType, QWidget*widgetPtr);


    void slotOnCloseCheckFinished(bool isAllOk);

    void slotDisableMenus(QVariant);
    void slotEnabledMenus(QVariant);

    void slotInitiateEwoFinisher(QAction *initiatorAction);
    void slotOnAppClose();

    void saveLastOpenedScenarioId(uint32_t scenarioId);
    uint32_t getLastOpenedScenarioId();

private:
    bool addPluginToMenu(CtrAppVisualizerPlugin* plugin, QMenu* menu);
    QWidget *create2DViszualizer();

    GlobalMessageNotifier * m_pGlobNotif;
//    ScenarioSelector *m_pScenarioSelector;
//    ManagerVectorObjectPlugin *m_pObjTree;
//    QAction *m_pShowAct, *m_pCloseAct, *m_pCreateAct, *m_pEditAct, *m_pDeleteAct, *m_pExitAct;
    QAction *m_pOpenFile, *m_pOpenStream, *m_pObjectBrowserAct, *m_p3DAct;
    QAction *m_pAppSettings/*, *m_pStyleDebug*/;
    QAction *m_pManuals, *m_pAbout;
    EndWorkingObject *m_ewoFinisher = nullptr;
    quint64 m_openScenarioWId, m_createScenarioWId, m_editScenarioWId;
    quint64 mainWindowId;
    QString m_applicationName;
    QList<CtrAppVisualizerPlugin*> m_pAppPlugins;
    QAction *initiatorActionForEwoFinisher = nullptr;

};

#endif // APPCOMPOSER_H
