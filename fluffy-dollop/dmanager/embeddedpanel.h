#ifndef EMBEDDEDPANEL_H
#define EMBEDDEDPANEL_H

#include <QQueue>
#include <QVBoxLayout>
#include <QWidget>

#include "embeddedprivate.h"

// TODO Нагрузочный тест

namespace ew
{

class EmbeddedWidget;

/**
 * @brief The InsertPolicy enum
 * Политика сдвига виджетов, которые в этом нуждаются
 */
enum class InsertPolicy
{
    Auto,       /// Автоматическая: вычисляется наименьшая длина сдвига
    Horizontal, /// Горизонтальная: все виджеты сдвигаются вправо
    Vertical    /// Вертикальная: все виджеты сдвигаются вниз
};

class EmbeddedPanel : public QWidget
{
    Q_OBJECT

    friend class EmbeddedApp;
    friend class EmbeddedGroupWidget;
    friend class EmbeddedMainWindow;
    friend class EmbeddedWidget;

    bool m_personalPanel;

    // Список всех виджетов, находящихся на панели
    QList<ew::EmbeddedWidgetBaseStructPrivate *> m_widgetList;

protected:
    explicit EmbeddedPanel(QWidget * parent = 0);

    /**
     * @brief removeEmbeddedWidget Удаление виджета из панели. Виджет оказывается не привязанным к ней.
     * Родитель = 0.
     * @param embStrPriv
     */
    void removeEmbeddedWidget(ew::EmbeddedWidgetBaseStructPrivate * embStrPriv);

    /**
     * @brief clear Удаление всех виджетов из текущей панели. Все виджеты "выбрасываются" из нее.
     * Родителем становиться 0.
     */
    void clear();

    /**
     * @brief getWidgetList Получение списка всех виджетов.
     * @return
     */
    QList<EmbeddedWidgetBaseStructPrivate *> getWidgetList()
    {
        return m_widgetList;
    }

    /**
     * @brief highlightWidget Подсветка места вставки.
     * @param localPos Локальная на панели позиция желаемой вставки.
     * @param sz Размер вставляемого виджета.
     */
    void highlightWidget(QPoint localPos, QSize sz);

    /**
     * @brief highlightWidgetTurnOff Отключение последней подсветки.
     */
    void highlightWidgetTurnOff();

    /**
     * @brief canAdd Проверка возможности добавления в панель виджетов данного типа
     * @param type
     * @return true - можно добавить, false - иначе
     */
    bool canAdd(EmbeddedWidgetType type);

    /**
     * @brief Вставка виджета eWdg в панель по с координатам localPanelPos.
     * EmbeddedPanel становится родителем встраиваемого виджета
     * @param ewStructPriv
     * @param localPanelPos
     */
    void insertEmbeddedWidget(EmbeddedWidgetBaseStructPrivate * ewStructPriv,
                              QPoint localPanelPos);

    /**
     * @brief Вставка виджета в панель без проверок, допустимо наслоение.
     * @param ewStructPriv
     * @param localPanelPos
     */
    void insertEmbeddedWidgetForce(EmbeddedWidgetBaseStructPrivate * ewStructPriv,
                                   QPoint localPanelPos);

    /**
     * @brief optimizeSpace Функция оптимизации занимаего пространства на панели.
     * Автоматически переставляет все виджеты, встроенные в данную панель, по алгоритму
     * FCNR (Floating Ceil No Rotation).
     */
    void optimizeSpace();

    /**
     * @brief optimizeWidgetSizes Сжатие/растяжение (определяется автоматически) виджетов
     * с сохранением их пропорций. Умещает все текущие виджеты в панели.
     */
    void optimizeWidgetSizes();

    /**
     * @brief setWidget Устанавливает единственный виджет в пустую панель и растягивает по максимуму,
     * в соответствии с политикой
     * @param embStrPriv
     */
    void setWidget(ew::EmbeddedWidgetBaseStructPrivate * embStrPriv);

    QRect getContentRect();
    void autoResize();

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent * e);
    void showEvent(QShowEvent * e);

private:

    /// Значения шагов по осям для поиска оптимальной позиции без пересечений
    int m_minimumChildHeight;
    int m_minimumChildWidth;

    /**
     * @brief fixIntersectedWidgets Рекурсивный метод сдвига всех виджетов по заданной политике.
     * @param widget Виджет. относительно которого происходит сдвиг
     * @param intersectedWidgets Список виджетов, имеющих пересечение заданным.
     * Можно получить при помощи getIntersectedWidgets(...).
     * @param insertPolicy Политика вставки
     */
    void fixIntersectedWidgets(EmbeddedWidgetBaseStructPrivate * widget,
                               QQueue<EmbeddedWidgetBaseStructPrivate *> intersectedWidgets,
                               InsertPolicy insertPolicy);

    /**
     * @brief getIntersectedWidgets Получение списка (в данном случае представленным очередью) всех виджетов,
     * границы который пересекаются с данным виджетом widget на вход.
     * @param ewStructPriv
     * @return
     */
    QQueue<EmbeddedWidgetBaseStructPrivate *> getIntersectedWidgets(
        EmbeddedWidgetBaseStructPrivate * ewStructPriv);

    /**
     * @brief getOptimalPosition Выбор оптимальной позиции для виджета.
     * Координаты localPos сперва сдвигается по оси X максимально влево,
     * затем сдвигается по оси Y максимально вверх.
     * @param localPos
     * @return
     */
    QPoint getOptimalPosition(QPoint localPos) const;

    QSize getOptimalSize(EmbeddedWidgetBaseStructPrivate * ewStructPriv);

    EmbeddedWidgetBaseStructPrivate * childAtNotPreview(int x, int y) const;
    EmbeddedWidgetBaseStructPrivate * childAtNotPreview(QPoint pos) const;

    // Все для подсветки
    bool m_highlightSpace;
    QRect m_highlightRect;
    EmbeddedWidgetBaseStructPrivate * m_pHighlightedEW;

};
}


#endif // EMBEDDEDPANEL_H
