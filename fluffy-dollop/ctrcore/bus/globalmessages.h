#ifndef GLOBALMESSAGES_H
#define GLOBALMESSAGES_H

#include <QPointF>
#include <QEvent>

namespace globalbusmessages {

static const char* globalNamespace = "global";

enum GLOBAL_MESSAGE_TYPE
{
    GMT_SCENARIO_OPENNED,
    GMT_SCENARIO_CLOSED,
    GMT_SCENARIO_PREPARE_TO_CLOSE,
    GMT_RELOAD_SCENARIO,
    GMT_OPEN_SCENARIO,
    GMT_CLOSE_SCENARIO,
    GMT_EDIT_LIST_CHANGED,
    GMT_USER_ACTION_ON_MAP
};

struct UserActionOnMap
{
    UserActionOnMap()
    {
        type = QEvent::None;
        buttons = Qt::NoButton;
        modifiers = Qt::NoModifier;
        posf = QPointF(0,0);
        epsgCode = 4326;
        sceneId = 0;
        sceneType = 0;
        keybKey = Qt::Key_unknown;
    }

    QEvent::Type type;
    Qt::MouseButtons buttons;
    Qt::KeyboardModifiers modifiers;

    QPointF posf;
    int epsgCode;

    /*****/
    int sceneId;
    int sceneType;
    Qt::Key keybKey;

};

}

Q_DECLARE_METATYPE(globalbusmessages::UserActionOnMap)


#endif // GLOBALMESSAGES_H
