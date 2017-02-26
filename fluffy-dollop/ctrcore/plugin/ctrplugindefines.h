#ifndef CTRPLUGINDEFINES_H
#define CTRPLUGINDEFINES_H

#endif // CTRPLUGINDEFINES_H

#include <QString>
#include <QIcon>

namespace ctrplugin
{
/// Тег для подсистемы журналирования для определения компонента, отправляющего сообщение
#define CTRPLUGIN "CtrPlugin"

enum TYPE_PLUGIN
{
    /// плагины уровня приложения
    TYPE_APP
    /// плагины - визуализаторы
    , TYPE_VISUALIZER
    /// Плагины представления данных
    , TYPE_PROVIDER
    /// Плагины объектов ГДМ
    , TYPE_OBJECT
    /// Плагины приложения, встраеваемые в визуализатор
    , TYPE_APP_VISUALIZER
};

inline TYPE_PLUGIN getTypeByName(QString name)
{
    if(name == "application")
        return TYPE_APP;
    if(name == "object")
        return TYPE_OBJECT;
    if(name == "provider")
        return TYPE_PROVIDER;
    if(name == "visual")
        return TYPE_VISUALIZER;
}

inline QString getNameByType(TYPE_PLUGIN type)
{
    if(type == TYPE_APP)
        return "application";
    if(type == TYPE_OBJECT)
        return "object";
    if(type == TYPE_PROVIDER)
        return "provider";
    if(type == TYPE_VISUALIZER)
        return "visual";
}

/// Вареанты встраивания кнопки управления плагином
enum class IntegrateWay
{
    /// Встроить кнопку управления плагином
    ToolButton = 0,
    /// Встроить пункт меню управления плагином
    MenuItem = 1
};

struct InitPluginData
{
    InitPluginData() : isCheckable(false), integrateWay(IntegrateWay::ToolButton) {}

    QString buttonName;             // имя для кнопки по английский, используется в методах "isChecked" и "checked".
    QString translateButtonName;    // имя для кнопки по русский, используется в выпадающем меню контроллера выезжающей панели.
    QIcon iconForButtonOn;          // иконка для кнопки в отжатом состоянии.
    QIcon iconForButtonOff;         // иконка для кнопки в нажатом состоянии, если это допустимо (при isCheckable == TRUE).
    QString tooltip;                // описание плагина по русский для всплывающей подсказки.
    bool isCheckable;               // флажок нажимаемости кнопки.
    IntegrateWay integrateWay;      // Флаг указывающий способ встраивания элемента управления плагином.
};

}
