#include "rb_meta_constraints.h"

#include <QJsonDocument>
#include <QVariantMap>
#include <QDir>

#include "rb_base_entity.h"
#include "rb_locations.h"
#include "rb_marks.h"

using namespace regionbiz;

Constraint::Constraint( Constraint::ConstraintType type,
                        QString name,
                        QString meta_type,
                        QString str,
                        QString showed_name,
                        QVariant default_val ):
    _meta_name( name ),
    _constrait( str ),
    _type( type ),
    _meta_type( meta_type ),
    _meta_showed_name( showed_name ),
    _default_value( default_val )
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

void Constraint::setMetaType(QString type)
{
    _meta_type = type;
}

QString Constraint::getMetaType()
{
    return _meta_type;
}

void Constraint::setShowedName(QString name)
{
    _meta_showed_name = name;
}

QString Constraint::getShowedName()
{
    return _meta_showed_name;
}

void Constraint::setDefaultValue(QVariant val)
{
    _default_value = val;
}

QVariant Constraint::getDefaultValue()
{
    return _default_value;
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

Constraint::ConstraintType Constraint::getType()
{
    return _type;
}

QString Constraint::separator()
{
    return CONSTRAINT_SEPARATOR;
}

//-----------------------------------

void ConstraintsManager::init( QString aim, QString file )
{
    // metadata that always need
    Constraint cons( Constraint::CT_SYSTEM, "number", "int", "0:" );
    addConstraint( BaseArea::AT_FLOOR, cons );

    Constraint cons_lift_stairs( Constraint::CT_SYSTEM, "", "string", "^(\\d+:)*\\d+$" );
    addConstraint( Mark::MT_STAIRS, cons_lift_stairs );
    addConstraint( Mark::MT_LIFT, cons_lift_stairs );

    // for rent
    if( aim == "rent" )
    {
        Constraint cons( Constraint::CT_SYSTEM, "area", "double", "0:" );
        addConstraint( BaseArea::AT_ROOM, cons );
    }
    else if( aim == "file" )
    {
        // add constraint by file
        ConstraintsManager::loadConstraintsFromFile( file );
    }
}

void ConstraintsManager::loadConstraintsFromFile( QString file_path )
{
    // replace '~' in config
    if( file_path.startsWith ( "~/" ))
        file_path.replace (0, 1, QDir::homePath());

    // open
    QString file_in;
    QFile file( file_path );
    if( file.open( QFile::ReadOnly | QFile::Text ))
    {
        file_in = file.readAll();
        file.close();
    }
    else
    {
        std::cerr << "Constraints can't open config file: "
                  << file_path.toUtf8().data() << std::endl;
    }

    // parce json
    QJsonParseError err;
    QJsonDocument json_doc = QJsonDocument::fromJson( file_in.toUtf8(), &err );
    if( err.error != QJsonParseError::NoError )
        std::cerr << "Error on parce constraints: "
                  << err.errorString().toUtf8().data();

    // return result
    QVariantMap map = json_doc.toVariant().toMap();
    loadConstraintsFromMap( map );
}

void ConstraintsManager::loadConstraintsFromMap( QVariantMap map )
{
    QVariantList constraints_list = map["constraints"].toList();
    for( QVariant cons_var: constraints_list )
    {
        QVariantMap cons_map = cons_var.toMap();
        if( !cons_map.contains( "type" )
                || !cons_map.contains( "entity_type" )
                || !cons_map.contains( "name" )
                || !cons_map.contains( "meta_type" )
                || !cons_map.contains( "constraint" ))
        {
            using namespace std;
            cerr << "Error on load constraint" << endl;
            continue;
        }

        QString entity_type = cons_map[ "entity_type" ].toString();
        QString type_str = cons_map[ "type" ].toString();
        Constraint::ConstraintType type = getTypeByString( type_str );
        QString name = cons_map[ "name" ].toString();
        QString cons_str = cons_map[ "constraint" ].toString();
        QString meta_type = cons_map[ "meta_type" ].toString();
        QString showed = cons_map[ "showed_name" ].toString();
        QVariant default_val = cons_map[ "default" ];

        // make
        Constraint cons( type, name, meta_type, cons_str );
        if( !showed.isEmpty() )
            cons.setShowedName( showed );
        if( default_val.isValid() )
            cons.setDefaultValue( default_val );

        // add to vectors
        if( "group" == entity_type )
            addConstraint( BaseEntity::ET_GROUP, cons );
        else if( "region" == entity_type )
            addConstraint( BaseArea::AT_REGION, cons );
        else if( "location" == entity_type )
            addConstraint( BaseArea::AT_LOCATION, cons );
        else if( "facility" == entity_type )
            addConstraint( BaseArea::AT_FACILITY, cons );
        else if( "floor" == entity_type )
            addConstraint( BaseArea::AT_FLOOR, cons );
        else if( "room" == entity_type )
            addConstraint( BaseArea::AT_ROOM, cons );
        else if( "defect" == entity_type )
            addConstraint( Mark::MT_DEFECT, cons );
        else if( "photo" == entity_type )
            addConstraint( Mark::MT_PHOTO, cons );
        else if( "photo_3d" == entity_type )
            addConstraint( Mark::MT_PHOTO_3D, cons );
        else if( "graph" == entity_type )
            addConstraint( BaseEntity::ET_GRAPH, cons );
        else if( "graph_node" == entity_type )
            addConstraint( BaseEntity::ET_GRAPH_NODE, cons );
        else if( "graph_edge" == entity_type )
            addConstraint( BaseEntity::ET_GRAPH_EDGE, cons );

        // TODO other type of entity, when read constraints from file
    }
}

Constraint::ConstraintType ConstraintsManager::getTypeByString( QString type )
{
    if( type == "free" )
        return Constraint::CT_FREE;
    if( type == "user" )
        return Constraint::CT_USER;
    if( type == "system" )
        return Constraint::CT_SYSTEM;

    std::cerr << "Can't define type of constrint: "
              << type.toUtf8().data() << std::endl;
    return Constraint::CT_FREE;
}



