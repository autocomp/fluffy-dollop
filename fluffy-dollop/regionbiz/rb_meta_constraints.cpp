#include "rb_meta_constraints.h"

#include "rb_base_entity.h"
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

void ConstraintsManager::init( QString /*aim*/ )
{
    Constraint cons( "area", "0:" );
    addConstraint( BaseEntity::ET_AREA, cons );

    // TODO add constraint by aim and by file
}

template<typename Type>
void ConstraintsManager::addConstraint(Type type, Constraint constr)
{
    auto contaier = ConstraintsManager::containerInstance< Type >();
    contaier[ type ].push_back( constr );
}

template<typename Type>
std::map< Type, std::vector< Constraint >> &ConstraintsManager::containerInstance()
{
    static std::map< Type, std::vector< Constraint >> contaier;
    return contaier;
}

template<typename Type>
std::vector<Constraint> ConstraintsManager::getConstraints(Type type)
{
    std::vector<Constraint> res;

    auto contaier = ConstraintsManager::containerInstance< Type >();
    if( contaier.find( type ) != contaier.end() )
        res = contaier[ type ];

    return res;
}
