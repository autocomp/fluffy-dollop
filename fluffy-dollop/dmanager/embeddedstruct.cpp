#include "embeddedstruct.h"

#include <QMenuBar>

#include "embeddedapp.h"
#include "embeddedsubiface.h"
#include "embeddedwidget.h"

using namespace ew;


EmbeddedWidgetBaseStruct::EmbeddedWidgetBaseStruct()
{
    isModal = false;
    header           = EmbeddedHeaderStruct();

    /*параметры EmbeddedWidget*/
    alignPoint = QPoint(-1, -1);
    alignType = EWA_IN_LEFT_TOP_CORNER_OFFSET;
    allowMoving = true;
    allowChangeParent = false;
    locked = false;
    collapsed = false;
    //showWindowTitle = true;
    addHided = false;
    szPolicy = ESP_EXPAND_FREE;
    size = QSize(0, 0);
    minSize = QSize(0, 0);
    maxSize = QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    isModal = false;
    headerVisible = true;
    topOnHint = false;
    allowGrouping = false;
    isModalBlock = true;
    maximized = false;
    autoAdjustSize = false;
    stylesheetFile = ewApp()->getMainStylesheetFile();
}


ew::EmbeddedWidgetBaseStruct & EmbeddedWidgetBaseStruct::operator=(const ew::EmbeddedWidgetBaseStruct & prev)
{
    isModal = prev.isModal;
    alignPoint = prev.alignPoint;
    alignType = prev.alignType;
    addHided = prev.addHided;
    size = prev.size;
    widgetTag = prev.widgetTag;
    locked = prev.locked;
    allowChangeParent = prev.allowChangeParent;
    collapsed = prev.collapsed;
    szPolicy = prev.szPolicy;
    header = prev.header;
    minSize = prev.minSize;
    maxSize = prev.maxSize;
    headerVisible = prev.headerVisible;
    stylesheetFile = prev.stylesheetFile;
    topOnHint = prev.topOnHint;
    autoAdjustSize = prev.autoAdjustSize;
    allowGrouping = prev.allowGrouping;
    maximized = prev.maximized;
    isModalBlock = prev.isModalBlock;

    return *this;
}


bool EmbeddedWidgetBaseStruct::operator!=( const EmbeddedWidgetBaseStruct & prev ) const
{
    if( isModal != prev.isModal ||
        alignPoint != prev.alignPoint ||
        alignType != prev.alignType ||
        allowMoving != prev.allowMoving ||
        addHided != prev.addHided ||
        size != prev.size ||
        widgetTag != prev.widgetTag ||
        locked != prev.locked ||
        collapsed != prev.collapsed ||
        allowChangeParent != prev.allowChangeParent ||
        widgetTag != prev.widgetTag ||
        szPolicy != prev.szPolicy ||
        header != prev.header ||
        minSize != prev.minSize ||
        maxSize != prev.maxSize ||
        headerVisible != prev.headerVisible ||
        stylesheetFile != prev.stylesheetFile ||
        topOnHint != prev.topOnHint ||
        allowGrouping != prev.allowGrouping ||
        maximized != prev.maximized ||
        autoAdjustSize != prev.autoAdjustSize ||
        isModalBlock != prev.isModalBlock)
    {
        return true;
    }

    return false;
}


bool EmbeddedWidgetBaseStruct::equal( const EmbeddedWidgetBaseStruct & prev ) const
{
    if(
        isModal == prev.isModal &&
        alignPoint == prev.alignPoint &&
        alignType == prev.alignType &&
        allowMoving == prev.allowMoving &&
        addHided == prev.addHided &&
        size == prev.size &&
        widgetTag == prev.widgetTag &&
        locked == prev.locked &&
        allowChangeParent == prev.allowChangeParent &&
        collapsed == prev.collapsed &&
        widgetTag == prev.widgetTag &&
        szPolicy == prev.szPolicy &&
        header == prev.header &&
        minSize == prev.minSize &&
        maxSize == prev.maxSize &&
        headerVisible == prev.headerVisible &&
        stylesheetFile == prev.stylesheetFile &&
        topOnHint == prev.topOnHint &&
        allowGrouping == prev.allowGrouping &&
        maximized == prev.maximized &&
        autoAdjustSize == prev.autoAdjustSize &&
        isModalBlock == prev.isModalBlock)
    {
        return true;
    }

    return false;
}


bool EmbeddedWidgetBaseStruct::operator==( const EmbeddedWidgetBaseStruct & prev ) const
{
    if( isModal == prev.isModal &&
        alignPoint == prev.alignPoint &&
        alignType == prev.alignType &&
        allowMoving == prev.allowMoving &&
        addHided == prev.addHided &&
        size == prev.size &&
        allowChangeParent == prev.allowChangeParent &&
        widgetTag == prev.widgetTag &&
        locked == prev.locked &&
        collapsed == prev.collapsed &&
        szPolicy == prev.szPolicy &&
        header == prev.header &&
        minSize == prev.minSize &&
        maxSize == prev.maxSize &&
        headerVisible == prev.headerVisible &&
        stylesheetFile == prev.stylesheetFile &&
        topOnHint == prev.topOnHint &&
        allowGrouping == prev.allowGrouping &&
        maximized == prev.maximized &&
        autoAdjustSize == prev.autoAdjustSize &&
        isModalBlock == prev.isModalBlock)
    {
        return true;
    }

    return false;
}


EmbeddedWidgetBaseStruct::EmbeddedWidgetBaseStruct(const EmbeddedWidgetBaseStruct & prev)
{
    isModal = prev.isModal;
    alignPoint = prev.alignPoint;
    alignType = prev.alignType;
    allowMoving = prev.allowMoving;
    addHided = prev.addHided;
    size = prev.size;
    locked = prev.locked;
    collapsed = prev.collapsed;
    widgetTag = prev.widgetTag;
    szPolicy = prev.szPolicy;
    header = prev.header;
    allowChangeParent = prev.allowChangeParent;
    minSize = prev.minSize;
    maxSize = prev.maxSize;
    headerVisible = prev.headerVisible;
    stylesheetFile = prev.stylesheetFile;
    topOnHint = prev.topOnHint;
    allowGrouping = prev.allowGrouping;
    maximized = prev.maximized;
    autoAdjustSize = prev.autoAdjustSize;
    isModalBlock = prev.isModalBlock;
}


EmbeddedWidgetStruct::EmbeddedWidgetStruct() :
    EmbeddedWidgetBaseStruct()
{
    iface = 0;
}


ew::EmbeddedWidgetStruct & EmbeddedWidgetStruct::operator=(const ew::EmbeddedWidgetStruct & prev)
{
    EmbeddedWidgetBaseStruct::operator=(static_cast<ew::EmbeddedWidgetBaseStruct>(prev));
    iface = prev.iface;

    return *this;
}


bool EmbeddedWidgetStruct::operator!=( const EmbeddedWidgetStruct & prev ) const
{
    bool res =     ( (EmbeddedWidgetBaseStruct::operator!=)(static_cast<ew::EmbeddedWidgetBaseStruct>(prev)));

    if(res || (iface != prev.iface))
    {
        return true;
    }

    return false;
}


bool EmbeddedWidgetStruct::equal( const EmbeddedWidgetStruct & prev ) const
{
    bool res = EmbeddedWidgetBaseStruct::equal(prev);

    if(res && (iface == prev.iface))
    {
        return true;
    }

    return false;
}


bool EmbeddedWidgetStruct::operator==( const EmbeddedWidgetStruct & prev ) const
{
    bool res = (EmbeddedWidgetBaseStruct::operator==(prev));

    if(res && iface == prev.iface)
    {
        return true;
    }

    return false;
}


EmbeddedWidgetStruct::EmbeddedWidgetStruct(const EmbeddedWidgetStruct & prev) :
    EmbeddedWidgetBaseStruct(prev)
{
    iface = prev.iface;
}


EmbeddedHeaderStruct::EmbeddedHeaderStruct()
{
    /*Параметры EmbeddedHeader*/
    hasSettingsButton                     = false;
    hasStyleButton                        = false;
    hasCloseButton                        = true;
    hasGlobalMoveButton                   = false;
    hasCollapseButton                     = false;
    hasPinButton                          = false;
    hasWhatButton                         = false;
    hasLockButton                         = false;
    hasHideHeaderButton                   = false;
    hasMinMaxButton                       = false;
    hasMidgetButton                       = false;
    descr                 = "";
    windowTitle                 = "";
    tooltip.clear();
    headerPixmap = "";
    align = EHA_TOP;
    visibleCbText = "";
}


EmbeddedHeaderStruct::EmbeddedHeaderStruct(const EmbeddedHeaderStruct & prev)
{
    hasSettingsButton = prev.hasSettingsButton;
    hasStyleButton = prev.hasStyleButton;
    hasMinMaxButton = prev.hasMinMaxButton;
    hasCloseButton = prev.hasCloseButton;
    //hasVisibilityButton = prev.hasVisibilityButton;
    hasGlobalMoveButton = prev.hasGlobalMoveButton;
    //hasVisibilityCheckbox = prev.hasVisibilityCheckbox;
    hasCollapseButton     = prev.hasCollapseButton;
    hasPinButton          = prev.hasPinButton;
    hasWhatButton         = prev.hasWhatButton;
    hasLockButton         = prev.hasLockButton;
    descr                 = prev.descr;
    windowTitle           = prev.windowTitle;
    tooltip               = prev.tooltip;
    headerPixmap          = prev.headerPixmap;
    align                 = prev.align;
    hasMidgetButton       = prev.hasMidgetButton;
    //hideHeader            = prev.hideHeader;
    hasHideHeaderButton   = prev.hasHideHeaderButton;
    visibleCbText = prev.visibleCbText;
}


EmbeddedHeaderStruct & EmbeddedHeaderStruct::operator=(const EmbeddedHeaderStruct & prev)
{
    hasSettingsButton     = prev.hasSettingsButton;
    hasStyleButton        = prev.hasStyleButton;
    hasCloseButton        = prev.hasCloseButton;
    //hasVisibilityButton   = prev.hasVisibilityButton;
    hasGlobalMoveButton   = prev.hasGlobalMoveButton;
    //hasVisibilityCheckbox = prev.hasVisibilityCheckbox;
    hasCollapseButton     = prev.hasCollapseButton;
    hasPinButton          = prev.hasPinButton;
    hasWhatButton         = prev.hasWhatButton;
    hasLockButton         = prev.hasLockButton;
    descr                 = prev.descr;
    windowTitle           = prev.windowTitle;
    tooltip               = prev.tooltip;
    headerPixmap          = prev.headerPixmap;
    hasMinMaxButton       = prev.hasMinMaxButton;
    align                 = prev.align;
    hasMidgetButton             = prev.hasMidgetButton;
    //hideHeader            = prev.hideHeader;
    hasHideHeaderButton   = prev.hasHideHeaderButton;
    visibleCbText = prev.visibleCbText;

    return *this;
}


bool EmbeddedHeaderStruct::operator!=( const EmbeddedHeaderStruct & prev ) const
{
    if(
        (hasCloseButton         != prev.hasCloseButton) ||
        //(hasVisibilityButton    != prev.hasVisibilityButton)||
        (hasGlobalMoveButton    != prev.hasGlobalMoveButton) ||
        //(hasVisibilityCheckbox  != prev.hasVisibilityCheckbox)||
        (hasCollapseButton      != prev.hasCollapseButton) ||
        (hasPinButton           != prev.hasPinButton) ||
        (hasWhatButton          != prev.hasWhatButton) ||
        (hasLockButton          != prev.hasLockButton) ||
        (descr                  != prev.descr) ||
        (windowTitle            != prev.windowTitle) ||
        (tooltip                != prev.tooltip) ||
        (align                  != prev.align) ||
        (hasMinMaxButton        != prev.hasMinMaxButton) ||
        (hasSettingsButton      != prev.hasSettingsButton) ||
        (hasStyleButton         != prev.hasStyleButton) ||
        (hasMidgetButton        != prev.hasMidgetButton) ||
        //(hideHeader             != prev.hideHeader)||
        (hasHideHeaderButton    != prev.hasHideHeaderButton) ||
        (visibleCbText != prev.visibleCbText)
        )

    {
        return true;
    }

    return false;
}


bool EmbeddedHeaderStruct::operator==( const EmbeddedHeaderStruct & prev ) const
{
    if(*this != prev)
    {
        return false;
    }

    return true;
}


bool EmbeddedHeaderStruct::equal( const EmbeddedHeaderStruct & prev ) const
{
    if(
        (hasCloseButton != prev.hasCloseButton) ||
        //(hasVisibilityButton != prev.hasVisibilityButton) ||
        (hasGlobalMoveButton != prev.hasGlobalMoveButton) ||
        //(hasVisibilityCheckbox != prev.hasVisibilityCheckbox) ||
        (hasCollapseButton != prev.hasCollapseButton) ||
        (hasPinButton != prev.hasPinButton) ||
        (hasWhatButton != prev.hasWhatButton) ||
        (hasLockButton != prev.hasLockButton) ||
        (descr != prev.descr) ||
        (windowTitle != prev.windowTitle) ||
        (tooltip != prev.tooltip) ||
        (align != prev.align) ||
        hasMinMaxButton != prev.hasMinMaxButton ||
        hasSettingsButton != prev.hasSettingsButton ||
        hasStyleButton != prev.hasStyleButton ||
        (hasMidgetButton != prev.hasMidgetButton) ||
        //(hideHeader != prev.hideHeader) ||
        (hasHideHeaderButton != prev.hasHideHeaderButton) ||
        (visibleCbText != prev.visibleCbText)

        /*(headerPixmap != prev.headerPixmap)*/ )
    {
        return false;
    }

    return true;
}


EmbeddedMainWindowStruct::EmbeddedMainWindowStruct() :
    EmbeddedGroupStruct()
{
    menuBar = 0;
    bottomWidget = 0;
}


ew::EmbeddedMainWindowStruct & EmbeddedMainWindowStruct::operator=(const ew::EmbeddedMainWindowStruct & prev)
{
    EmbeddedGroupStruct::operator=(static_cast<ew::EmbeddedGroupStruct>(prev));
    menuBar = prev.menuBar;
    bottomWidget = prev.bottomWidget;
    return *this;
}


bool EmbeddedMainWindowStruct::operator!=( const EmbeddedMainWindowStruct & prev ) const
{
    bool res =     ( (EmbeddedGroupStruct::operator!=)(static_cast<ew::EmbeddedGroupStruct>(prev)));

    if(res || (bottomWidget != prev.bottomWidget) || (menuBar != prev.menuBar) )
    {
        return true;
    }

    return false;
}


bool EmbeddedMainWindowStruct::equal( const EmbeddedMainWindowStruct & prev ) const
{
    bool res = EmbeddedGroupStruct::equal(prev);

    if(res && (bottomWidget == prev.bottomWidget) && (menuBar == prev.menuBar))
    {
        return true;
    }

    return false;
}


bool EmbeddedMainWindowStruct::operator==( const EmbeddedMainWindowStruct & prev ) const
{
    bool res = (EmbeddedGroupStruct::operator==(prev));

    if(res && (bottomWidget == prev.bottomWidget) && (menuBar == prev.menuBar))
    {
        return true;
    }

    return false;
}


EmbeddedMainWindowStruct::EmbeddedMainWindowStruct(const EmbeddedMainWindowStruct & prev) :
    EmbeddedGroupStruct(prev)
{
    bottomWidget = prev.bottomWidget;
    menuBar = prev.menuBar;
}


EmbeddedGroupStruct::EmbeddedGroupStruct() :
    EmbeddedWidgetBaseStruct()
{
}


ew::EmbeddedGroupStruct & EmbeddedGroupStruct::operator=(const ew::EmbeddedGroupStruct & prev)
{
    EmbeddedWidgetBaseStruct::operator=(static_cast<ew::EmbeddedWidgetBaseStruct>(prev));
    m_widgets = prev.m_widgets;

    return *this;
}


bool EmbeddedGroupStruct::operator!=( const EmbeddedGroupStruct & prev ) const
{
    bool res =     ( (EmbeddedWidgetBaseStruct::operator!=)(static_cast<ew::EmbeddedWidgetBaseStruct>(prev)));

    if(res || (m_widgets != prev.m_widgets) /*|| (widgets != prev.widgets)*/)
    {
        return true;
    }

    return false;
}


bool EmbeddedGroupStruct::equal( const EmbeddedGroupStruct & prev ) const
{
    bool res = EmbeddedWidgetBaseStruct::equal(prev);

    if(res && (m_widgets == prev.m_widgets) /*&& (widgets == prev.widgets)*/)
    {
        return true;
    }

    return false;
}


bool EmbeddedGroupStruct::operator==( const EmbeddedGroupStruct & prev ) const
{
    bool res = (EmbeddedWidgetBaseStruct::operator==(prev));

    if(res && (m_widgets == prev.m_widgets) /*&& (widgets == prev.widgets)*/)
    {
        return true;
    }

    return false;
}


EmbeddedGroupStruct::EmbeddedGroupStruct(const EmbeddedGroupStruct & prev) :
    EmbeddedWidgetBaseStruct(prev)
{
    m_widgets == prev.m_widgets;
}


EmbeddedWindowStruct::EmbeddedWindowStruct() :
    EmbeddedGroupStruct()
{
    iface = 0;
}


ew::EmbeddedWindowStruct & EmbeddedWindowStruct::operator=(const ew::EmbeddedWindowStruct & prev)
{
    EmbeddedGroupStruct::operator=(static_cast<ew::EmbeddedGroupStruct>(prev));
    iface = prev.iface;
    return *this;
}


bool EmbeddedWindowStruct::operator!=( const EmbeddedWindowStruct & prev ) const
{
    bool res =     ( (EmbeddedGroupStruct::operator!=)(static_cast<ew::EmbeddedGroupStruct>(prev)));

    if(res || (iface != prev.iface))
    {
        return true;
    }

    return false;
}


bool EmbeddedWindowStruct::equal( const EmbeddedWindowStruct & prev ) const
{
    bool res = EmbeddedGroupStruct::equal(prev);

    if(res && (iface == prev.iface))
    {
        return true;
    }

    return false;
}


bool EmbeddedWindowStruct::operator==( const EmbeddedWindowStruct & prev ) const
{
    bool res = (EmbeddedGroupStruct::operator==(prev));

    if(res && (iface == prev.iface))
    {
        return true;
    }

    return false;
}


EmbeddedWindowStruct::EmbeddedWindowStruct(const EmbeddedWindowStruct & prev) :
    EmbeddedGroupStruct(prev)
{
    iface = prev.iface;
}
