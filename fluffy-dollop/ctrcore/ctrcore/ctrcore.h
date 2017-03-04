#ifndef CTRCORE_H
#define CTRCORE_H

#include <QObject>
//#include <unilog/unilog.h>
#include <QTranslator>

namespace ctrcore
{

class CtrCore : public QObject
{
    Q_OBJECT
public:
    explicit CtrCore(QObject *parent = 0);

    virtual ~CtrCore();

    /**
     * @brief init метод последовательной инициализации след компонентов:
     *  - журналирование
     *  - инициализация пользовательских компонентов
     *  - управление конфигурации
     *  - управление ресурсами
     *  - управление компоновкой
     * Не рекомендуется переопределять данный метод
     * @return
     */
    virtual bool init();

    static void qtMessageHandler(QtMsgType msgType, const char *msg);

protected:

    /**
     * @brief initLogger метод для инициализации процесса журналирования работы приложения
     * @return TRUE если менеджер инициализирован без ошибок
     */
    virtual bool initLogger();

    /**
     * @brief initConfigs метод для инициализации менеджера конфигураций
     * @return TRUE если менеджер инициализирован без ошибок
     */
    virtual bool initConfigs();

    /**
     * @brief initRepresentationServer метод для инициализации сервера публикации данных
     * @return
     */
    virtual bool initRepresentationServer();

    virtual bool initEmbeddedLib();

private:
    void showErrorMessage(const QString &msg);
//    /// Смартпоинтер на логгер
//    unilog::LoggerPtr _logger;
//    /// Смартпоинтер на логгер для qDebug
//    unilog::LoggerPtr _qtLogger;
    QTranslator* translator = nullptr;
    QList<QTranslator*> translators;
};

} //end of namespace ctrcore

#endif // CTRCORE_H
