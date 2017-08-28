#include "rb_meta_constraints.h"

#include "rb_base_entity.h"
#include "rb_locations.h"
#include "rb_marks.h"

using namespace regionbiz;

Constraint::Constraint(QString name, QString str):
    _meta_name( name ),
    _constrait( str )
{}

Constraint::Constraint()
{}

void Constraint::setMetaName(QString name)
{
    _meta_name = name;
}

QString Constraint::getMetaName()
{
    return _meta_name;
}

void Constraint::setConstraint(QString constr)
{
    _constrait = constr;
}

QString Constraint::getConstraint()
{
    return _constrait;
}

QVariantList Constraint::getConstraintAsList()
{
    QStringList list = _constrait.split( separator(), QString::KeepEmptyParts );
    QVariantList res_list;
    for( QString str: list )
        res_list.push_back( str );

    return res_list;
}

QString Constraint::separator()
{
    return CONSTRAINT_SEPARATOR;
}

//-----------------------------------

void ConstraintsManager::init( QString aim )
{
    // metadata that always need
    Constraint cons( "number", "0:" );
    addConstraint( BaseArea::AT_FLOOR, cons );

    // for rent
    if( aim == "rent" )
    {
        Constraint cons( "area", "0:" );
        addConstraint( BaseEntity::ET_AREA, cons );
    }
    else if( aim == "file" )
    {
        // TODO add constraint by file
    }
}



