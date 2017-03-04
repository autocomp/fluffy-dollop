#include "embeddedprivate.h"

using namespace  ew;



ew::EmbeddedWidgetBaseStructPrivate::EmbeddedWidgetBaseStructPrivate()
{
    parentId        = 0;
    id              = 0;
    embWidgetStruct = 0;
    embWidgetPtr    = 0;
    ewType = ew::EWT_WIDGET;
}


EmbeddedWidgetBaseStructPrivate &EmbeddedWidgetBaseStructPrivate::operator=(const EmbeddedWidgetBaseStructPrivate &prev)
{
    parentId       = prev.parentId;
    id             = prev.id           ;
    embWidgetStruct= prev.embWidgetStruct;
    embWidgetPtr   = prev.embWidgetPtr ;
    ewType = prev.ewType;
    return *this;
}

bool EmbeddedWidgetBaseStructPrivate::operator ==(const EmbeddedWidgetBaseStructPrivate &prev) const
{
    if(*this != prev)
    {
        return false;
    }
    return true;
}


bool EmbeddedWidgetBaseStructPrivate::operator!= ( const EmbeddedWidgetBaseStructPrivate & prev ) const
{
    return !(this->equal(prev));
}


bool EmbeddedWidgetBaseStructPrivate::equal(const EmbeddedWidgetBaseStructPrivate &prev) const
{
    if(this->parentId !=prev.parentId
            || this->id             != prev.id
            || this->embWidgetPtr   != prev.embWidgetPtr
            || this->embWidgetStruct!= prev.embWidgetStruct
            || ewType != prev.ewType)

    {
        return false;
    }
    return true;
}


EmbeddedWidgetBaseStructPrivate::EmbeddedWidgetBaseStructPrivate(const EmbeddedWidgetBaseStructPrivate &prev)
{
    parentId       = prev.parentId       ;
    id             = prev.id             ;
    embWidgetPtr   = prev.embWidgetPtr   ;
    embWidgetStruct= prev.embWidgetStruct;
    ewType = prev.ewType;
}



ew::EmbeddedWidgetStructPrivate::EmbeddedWidgetStructPrivate() :
    EmbeddedWidgetBaseStructPrivate()
{
    embMidgetPtr    = 0;
}


EmbeddedWidgetStructPrivate &EmbeddedWidgetStructPrivate::operator=(const EmbeddedWidgetStructPrivate &prev)
{
    EmbeddedWidgetBaseStructPrivate::operator= (static_cast<ew::EmbeddedWidgetBaseStructPrivate>(prev));
    embMidgetPtr = prev.embMidgetPtr;

    return *this;
}

bool EmbeddedWidgetStructPrivate::operator ==(const EmbeddedWidgetStructPrivate &prev) const
{
    bool res =     ( (EmbeddedWidgetBaseStructPrivate::operator !=)(static_cast<ew::EmbeddedWidgetBaseStructPrivate>(prev)));

    if(res && (embMidgetPtr == prev.embMidgetPtr))
    {
        return true;
    }

    return false;
}


bool EmbeddedWidgetStructPrivate::operator!= ( const EmbeddedWidgetStructPrivate & prev ) const
{
    bool res =     ( (EmbeddedWidgetBaseStructPrivate::operator !=)(static_cast<ew::EmbeddedWidgetBaseStructPrivate>(prev)));

    if(res || (embMidgetPtr != prev.embMidgetPtr))
    {
        return true;
    }

    return false;
}


bool EmbeddedWidgetStructPrivate::equal(const EmbeddedWidgetStructPrivate &prev) const
{
    bool res = EmbeddedWidgetBaseStructPrivate::equal(prev);
    if(res && (embMidgetPtr == prev.embMidgetPtr))
    {
        return true;
    }

    return false;
}


EmbeddedWidgetStructPrivate::EmbeddedWidgetStructPrivate(const EmbeddedWidgetStructPrivate &prev):
        EmbeddedWidgetBaseStructPrivate(prev)
{
    embMidgetPtr = prev.embMidgetPtr;
}







ew::EmbeddedGroupStructPrivate::EmbeddedGroupStructPrivate() :
    EmbeddedWidgetStructPrivate()
{
    embMidgetPtr    = 0;
}


EmbeddedGroupStructPrivate &EmbeddedGroupStructPrivate::operator=(const EmbeddedGroupStructPrivate &prev)
{
    EmbeddedWidgetStructPrivate::operator= (static_cast<ew::EmbeddedWidgetStructPrivate>(prev));
    embMidgetPtr = prev.embMidgetPtr;

    return *this;
}

bool EmbeddedGroupStructPrivate::operator ==(const EmbeddedGroupStructPrivate &prev) const
{
    bool res =     ( (EmbeddedWidgetStructPrivate::operator !=)(static_cast<ew::EmbeddedWidgetStructPrivate>(prev)));

    if(res && (embMidgetPtr == prev.embMidgetPtr))
    {
        return true;
    }

    return false;
}


bool EmbeddedGroupStructPrivate::operator!= ( const EmbeddedGroupStructPrivate & prev ) const
{
    bool res =     ( (EmbeddedWidgetStructPrivate::operator !=)(static_cast<ew::EmbeddedWidgetStructPrivate>(prev)));

    if(res || (embMidgetPtr != prev.embMidgetPtr))
    {
        return true;
    }

    return false;
}


bool EmbeddedGroupStructPrivate::equal(const EmbeddedGroupStructPrivate &prev) const
{
    bool res = EmbeddedWidgetStructPrivate::equal(prev);
    if(res && (embMidgetPtr == prev.embMidgetPtr))
    {
        return true;
    }

    return false;
}


EmbeddedGroupStructPrivate::EmbeddedGroupStructPrivate(const EmbeddedGroupStructPrivate &prev):
        EmbeddedWidgetStructPrivate(prev)
{
    embMidgetPtr = prev.embMidgetPtr;
}






ew::EmbeddedWindowStructPrivate::EmbeddedWindowStructPrivate() :
    EmbeddedGroupStructPrivate()
{
    embMidgetPtr    = 0;
}


EmbeddedWindowStructPrivate &EmbeddedWindowStructPrivate::operator=(const EmbeddedWindowStructPrivate &prev)
{
    EmbeddedGroupStructPrivate::operator= (static_cast<ew::EmbeddedGroupStructPrivate>(prev));
    embMidgetPtr = prev.embMidgetPtr;

    return *this;
}

bool EmbeddedWindowStructPrivate::operator ==(const EmbeddedWindowStructPrivate &prev) const
{
    bool res =     ( (EmbeddedGroupStructPrivate::operator !=)(static_cast<ew::EmbeddedGroupStructPrivate>(prev)));

    if(res && (embMidgetPtr == prev.embMidgetPtr))
    {
        return true;
    }

    return false;
}


bool EmbeddedWindowStructPrivate::operator!= ( const EmbeddedWindowStructPrivate & prev ) const
{
    bool res =     ( (EmbeddedGroupStructPrivate::operator !=)(static_cast<ew::EmbeddedGroupStructPrivate>(prev)));

    if(res || (embMidgetPtr != prev.embMidgetPtr))
    {
        return true;
    }

    return false;
}


bool EmbeddedWindowStructPrivate::equal(const EmbeddedWindowStructPrivate &prev) const
{
    bool res = EmbeddedGroupStructPrivate::equal(prev);
    if(res && (embMidgetPtr == prev.embMidgetPtr))
    {
        return true;
    }

    return false;
}


EmbeddedWindowStructPrivate::EmbeddedWindowStructPrivate(const EmbeddedWindowStructPrivate &prev):
        EmbeddedGroupStructPrivate(prev)
{
    embMidgetPtr = prev.embMidgetPtr;
}


