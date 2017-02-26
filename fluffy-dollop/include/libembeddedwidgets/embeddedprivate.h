#ifndef EMBEDDEDPRIVATE_H
#define EMBEDDEDPRIVATE_H

#include "embeddedstruct.h"

namespace ew
{

class EmbeddedWidgetStruct;
class EmbeddedMidget;
class EmbeddedApp;
class EmbeddedGroupStruct;
class EmbeddedGroupWidget;
class EmbeddedMainWindow;
class EmbeddedWidget;
class EmbeddedWindow;

class EmbeddedWidgetBaseStructPrivate
{
    friend class EmbeddedApp;
    friend class EmbeddedMainWindow;
    friend class EmbeddedWindow;
    friend class EmbeddedWidget;
    friend class EmbeddedWidgetStructPrivate;
    friend class EmbeddedPanel;
    friend class EmbeddedSessionManager;

protected:

    EmbeddedWidgetBaseStructPrivate();
    EmbeddedWidgetBaseStructPrivate(const EmbeddedWidgetBaseStructPrivate& prev);
    EmbeddedWidgetBaseStructPrivate& operator=(const EmbeddedWidgetBaseStructPrivate& prev);
    bool operator!= ( const EmbeddedWidgetBaseStructPrivate & prev ) const;
    bool equal( const EmbeddedWidgetBaseStructPrivate & prev ) const;
    bool operator == ( const EmbeddedWidgetBaseStructPrivate & prev ) const;
    virtual ew::EmbeddedWidgetType type(){return ewType;}

    quint64 parentId;
    quint64 id;
    EmbeddedWidget       * embWidgetPtr;
    EmbeddedWidgetBaseStruct * embWidgetStruct;
    ew::EmbeddedWidgetType ewType;

    virtual ~EmbeddedWidgetBaseStructPrivate()
    {
        delete embWidgetStruct;
    }
};


///// TODO дописать деструктор
class EmbeddedWidgetStructPrivate : public EmbeddedWidgetBaseStructPrivate
{
    friend class EmbeddedApp;
    friend class EmbeddedMainWindow;
    friend class EmbeddedWindow;
    friend class EmbeddedWidget;
    friend class EmbeddedPanel;
    friend class EmbeddedGroupStructPrivate;
    friend class EmbeddedSessionManager;


protected:

    EmbeddedWidgetStructPrivate();
    ew::EmbeddedWidgetStructPrivate& operator=(const ew::EmbeddedWidgetStructPrivate& prev);
    bool operator!= ( const EmbeddedWidgetStructPrivate & prev ) const;
    bool equal( const EmbeddedWidgetStructPrivate & prev ) const;
    bool operator == ( const EmbeddedWidgetStructPrivate & prev ) const;
    EmbeddedWidgetStructPrivate(const EmbeddedWidgetStructPrivate& prev);
    EmbeddedMidget       * embMidgetPtr;
};



/// TODO дописать деструктор
class EmbeddedGroupStructPrivate : public EmbeddedWidgetStructPrivate
{

    friend class EmbeddedApp;
    friend class EmbeddedMainWindow;
    friend class EmbeddedWindow;
    friend class EmbeddedWindowStructPrivate;

protected:

    EmbeddedGroupStructPrivate();
    ew::EmbeddedGroupStructPrivate& operator=(const ew::EmbeddedGroupStructPrivate& prev);
    bool operator!= ( const EmbeddedGroupStructPrivate & prev ) const;
    bool equal( const EmbeddedGroupStructPrivate & prev ) const;
    bool operator == ( const EmbeddedGroupStructPrivate & prev ) const;
    EmbeddedGroupStructPrivate(const EmbeddedGroupStructPrivate& prev);


};



/// TODO дописать деструктор
class EmbeddedWindowStructPrivate : public EmbeddedGroupStructPrivate
{
    //Q_OBJECT

    friend class EmbeddedApp;
    friend class EmbeddedMainWindow;
    friend class EmbeddedWindow;

protected:

    EmbeddedWindowStructPrivate();
    ew::EmbeddedWindowStructPrivate& operator=(const ew::EmbeddedWindowStructPrivate& prev);
    bool operator!= ( const EmbeddedWindowStructPrivate & prev ) const;
    bool equal( const EmbeddedWindowStructPrivate & prev ) const;
    bool operator == ( const EmbeddedWindowStructPrivate & prev ) const;
    EmbeddedWindowStructPrivate(const EmbeddedWindowStructPrivate& prev);

};
}


#endif // EMBEDDEDPRIVATE_H
