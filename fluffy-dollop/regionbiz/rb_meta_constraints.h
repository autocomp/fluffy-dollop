#ifndef RB_META_CONSTRAINTS_H
#define RB_META_CONSTRAINTS_H

#include <QVariantList>

#define CONSTRAINT_SEPARATOR ":";

namespace regionbiz {

class Constraint
{
public:
    enum ConstraintType
    {
        CT_FREE,
        CT_SYSTEM,
        CT_USER
    };

    Constraint( ConstraintType type, QString name,
                QString meta_type, QString str,
                QString showed_name = "", QVariant default_val = QVariant() );
    Constraint();

    void setMetaName( QString name );
    QString getMetaName();
    void setMetaType( QString type );
    QString getMetaType();
    void setShowedName( QString name );
    QString getShowedName();
    void setDefaultValue( QVariant val );
    QVariant getDefaultValue();

    void setConstraint( QString constr );
    QString getConstraint();
    QVariantList getConstraintAsList();
    ConstraintType getType();

    static QString separator();

private:
    QString _meta_name = "";
    QString _constrait = "";
    QString _meta_type = "";
    QString _meta_showed_name = "";
    QVariant _default_value = QVariant();
    ConstraintType _type;
};
typedef std::vector< Constraint > Constraints;

//-----------------------------------

class ConstraintsManager
{
public:
    /**
     * @brief init init by aim
     * @param aim - main theme of app config
     * - rent - for rent
     * - file - by file
     */
    static void init(QString aim , QString file);

    template< typename Type >
    static bool addConstraint( Type type, Constraint constr )
    {
        auto& contaier = ConstraintsManager::containerInstance< Type >();
        for( Constraint& cons: contaier[ type ] )
            if( cons.getMetaName() == constr.getMetaName() )
            {
                if( cons.getType() == constr.getType()
                        && cons.getMetaType() == constr.getMetaType() )
                {
                    cons = constr;
                    return true;
                }
                else
                    return false;
            }

        contaier[ type ].push_back( constr );
        return true;
    }

    template< typename Type >
    static std::vector< Constraint > getConstraints( Type type )
    {
        std::vector<Constraint> res;

        auto contaier = ConstraintsManager::containerInstance< Type >();
        if( contaier.find( type ) != contaier.end() )
            res = contaier[ type ];

        return res;
    }

    template< typename Type >
    static std::vector< Constraint > getConstraints( Type type,
                                                     Constraint::ConstraintType cons_type )
    {
        std::vector<Constraint> res;
        std::vector<Constraint> all;

        auto contaier = ConstraintsManager::containerInstance< Type >();
        if( contaier.find( type ) != contaier.end() )
            all = contaier[ type ];

        for( Constraint cons: all )
            if( cons.getType() == cons_type )
                res.push_back( cons );

        return res;
    }

    template< typename Type >
    static bool isConstraintPresent( Type type, QString name )
    {
        auto contaier = ConstraintsManager::containerInstance< Type >();
        if( contaier.find( type ) != contaier.end() )
            return false;

        for( Constraint& cons: contaier[ type ] )
            if( cons.getMetaName() == name )
                return true;

        return false;
    }

    template< typename Type >
    static Constraint getConstraint( Type type, QString name )
    {
        auto contaier = ConstraintsManager::containerInstance< Type >();
        if( contaier.find( type ) != contaier.end() )
            return Constraint( Constraint::CT_FREE, "", "", "" );

        for( Constraint& cons: contaier[ type ] )
            if( cons.getMetaName() == name )
                return cons;

        return Constraint( Constraint::CT_FREE, "", "", "" );
    }

private:
    template< typename Type >
    static std::map< Type, std::vector< Constraint >>& containerInstance()
    {
        static std::map< Type, std::vector< Constraint >> contaier;
        return contaier;
    }

    static void loadConstraintsFromFile( QString file_path );
    static void loadConstraintsFromMap( QVariantMap map );
    static Constraint::ConstraintType getTypeByString( QString type );
};

}

#endif // RB_META_CONSTRAINTS_H
