#ifndef EMBEDDEDSIRENIFACE_H
#define EMBEDDEDSIRENIFACE_H

#include <QMap>
#include <QProcess>
#include <QVariant>
#include <QWidget>
#include <QtGlobal>

namespace ew {
/**
 * @brief Интерфейс для объекта, желающего получать обновления от привязанного виджета
 */
class EmbeddedSubIFace
{
protected:

#ifdef QT_V5
    quint64 m_wId = 0;
    QWindow * m_pExtWindow = 0;
#endif
public:
    EmbeddedSubIFace();
    virtual ~EmbeddedSubIFace();

    /**
     * @brief Возвращает id связанного с данным интерфейсом
     * @return
     */
    quint64 id();


    /**
     * @brief Функция вызывается при изменении видимости сопутствующего окна EmbeddedWidget
     * @param vis - принятая видимость элемента
     * @param accept - указатель на переменную, разрешающую или запрещающую данную операцию
     */

    virtual void visibleChanged(bool vis, bool *accept){Q_UNUSED(vis); Q_UNUSED(accept)}


    /**
     * @brief Вызывается при закрытии окна(явное скрытие пользователем)
     * @param accept - указатель на переменную, разрешающую или запрещающую данную операцию
     */

    virtual void closed(bool *accept){Q_UNUSED(accept)}


    /**
     * @brief Вызывается при раскрытии окна на полный экран соответствующей кнопокй
     * @param accept - указатель на переменную, разрешающую или запрещающую данную операцию
     */

    virtual void maximized(bool *accept){Q_UNUSED(accept)}


    /**
     * @brief Восстановление окна в нормальные размеры(после maximize)
     * @param accept - указатель на переменную, разрешающую или запрещающую данную операцию
     */

    virtual void resored(bool *accept){Q_UNUSED(accept)}


    /**
     * @brief Сворачивание окна в панель задач
     * @param accept - указатель на переменную, разрешающую или запрещающую данную операцию
     */

    virtual void minimized(bool *accept){Q_UNUSED(accept)}


    /**
     * @brief Сворачивание окна до заголовка: на экране остается только заголовок
     * @param accept - указатель на переменную, разрешающую или запрещающую данную операцию
     */

    virtual void collapsed(bool *accept){Q_UNUSED(accept)}


    /**
     * @brief Блокировка виджета: запрет скрытия
     * @param accept - указатель на переменную, разрешающую или запрещающую данную операцию
     */

    virtual void locked(bool *accept){Q_UNUSED(accept)}


    /**
     * @brief Восстановление свойств, заполненных в прошлой сессии
     * @param propMap - карта свойств
     */

    virtual void restore(QMap<QString, QVariant> propMap){Q_UNUSED(propMap)}

    /**
     * @brief Возвращение указателя на виджета
     * @return
     */
    virtual QWidget *getWidget() = 0;

#ifdef QT_V5
        quint64 getWID();
        QWindow * getWindow();
#endif
};


/**
 * @brief Класс для вставки окон, не требующих оповещений о своех событиях
 */
class EmbeddedFace : public QObject, public EmbeddedSubIFace
{
    Q_OBJECT
    friend class EmbeddedApp;

    QWidget *m_pWidget;


public:
    explicit EmbeddedFace(QWidget *wdg);

#ifdef QT_V5
    explicit EmbeddedFace(QString appStr, QStringList args = QStringList(), QProcessEnvironment env = QProcessEnvironment());
    explicit EmbeddedFace(quint64 wId);

#endif
    QWidget *getWidget();

protected:
    quint64      getWidByPid(quint64 pid);
    virtual void restore(QMap<QString, QVariant> propMap);
    virtual void closed();
    virtual void visisbleChanged(bool vis);

signals:
    void signalRestore(QMap<QString, QVariant> );
    void signalClosed();
    void signalVisibleChanged(bool visisbility);
};
}

#endif // EMBEDDEDSIRENIFACE_H
