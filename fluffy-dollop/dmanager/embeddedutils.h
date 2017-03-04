#ifndef EMBEDDEDUTILS_H
#define EMBEDDEDUTILS_H

#include <QSize>
#include <QMargins>
#include <QRect>
#include <QHash>
#include "embeddedstruct.h"

namespace ew {

class EmbeddedUtils
{
public:
    EmbeddedUtils();

    /**
     * @brief Проверяет размеры виджета
     * @param settedSize - текущие размеры
     * @param minSize - минимальные размеры
     * @param maxSize - максимальные размеры
     * @return скорректированные размеры
     */
    static QSize getCorrectSize(const QSize& settedSize, const QSize& minSize, const QSize& maxSize);

    /**
     * @brief Возвращает размер с учетом Margins
     * @param sz - исходный размер view
     * @param parentMargins - размер границ
     * @return сложенные размеры
     */
    static QSize getEmbeddedSize(const QSize& viewSize, const QMargins& parentMargins);

    /**
     * @brief Конвертирует позицию виджета относительно родителя(относительно левого верхнего угла родителя) в alignPoint в соответсвии с указанной политикой
     * @param posOnParent - позиция виджета на родителе(если родителя нет - позиция на экране)
     * @param alignType - тип выравнивания
     * @param widgetSize - размер веджета
     * @param parentViewSize - размер родителя. В случае
     * , если политика внутренняя - указывается размер области, по которой может перемещаться дочерний виджет
     * @param parentPos - позиция родителя на экране(или своем родителе). Требуется только для внешней компоновки
     * @return - расчетная точка выравнивани.
     */
    static QPointF convertToAlignPoint(QPointF posOnParent, ew::EmbeddedWidgetAlign alignType
                                               , QSize widgetSize, QSize parentViewSize, QPointF parentPos );


    /**
     * @brief Конвертирует alignPoint виджета в позицию на родителе(относительно верхнего левого угла)
     * @param posOnParent - позиция виджета на родителе(если родителя нет - позиция на экране)
     * @param alignType - тип выравнивания
     * @param widgetSize - размер веджета
     * @param parentViewSize - размер родителя. В случае
     * , если политика внутренняя - указывается размер области, по которой может перемещаться дочерний виджет
     * @param parentPos - позиция родителя на экране(или своем родителе). Требуется только для внешней компоновки
     * @return - расчетная точка выравнивани.
     */
    static QPointF convertFromAlignPoint(QPointF alignPoint, ew::EmbeddedWidgetAlign alignType
                                               , QSize widgetSize, QSize parentViewSize, QPointF parentPos );

    /**
     * @brief Функция коррекции размеров/положения виджета.
     * @param parentRect - rect родителя, в котором разрешено перемещение виджета
     * @param widgetRect - rect виджета, относительно родителя
     * @param minSize - минимальный размер виджета
     * @param maxSize - максимальный размер виджета
     * @param saveSize - true - сохранение размеров виджета(по возможности). false - корректируется положение виджета
     * @return
     */
    static bool correctWidgetGeometry(QRect parentRect, QRect &widgetRect, QSize minSize, QSize maxSize, bool saveSize = true);

    /**
     * @brief Рекурсивное удаление непустой директории
     * @param dirName - полный путь к директории
     * @return true - в случае успешного удаления
     */
    static bool removeDir(const QString &dirName);

    /**
     * @brief Функция формирует hash строку
     * @param str - исходная строка
     * @return hash, записанный в виде строки
     */
    static QString hashString(const QString &str);


    /**
     * @brief Функция формирует hash строку
     * @param str - исходная строка
     * @return hash, записанный в виде числа
     */
    static qint64 hashInt64(const QString &str);

    /**
     * @brief Функция переводит русские символы в строке в символы транслита. Обратное преобразование !невозможно!
     * @param str - исходная строка. Символы в строке могут быть смешаны, транслируются только русские.
     * @return строка транслита.
     */
    static QString toTranslite(const QString &str);

    static QSize getWidgetSizeHint(QWidget * wdg);

private:
    static QHash<QString, QString> fillTrHash();
};
}

#endif // EMBEDDEDUTILS_H
