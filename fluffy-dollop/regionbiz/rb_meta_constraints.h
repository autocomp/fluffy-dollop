#ifndef RB_META_CONSTRAINTS_H
#define RB_META_CONSTRAINTS_H

#include <QVariantList>

#define CONSTRAINT_SEPARATOR ":";

namespace regionbiz {

class Constraint
{
public:
    Constraint( QString name, QString str );
    Constraint();

    void setMetaName( QString name );
    QString getMetaName();
    void setConstraint( QString constr );
    QString getConstraint();
    QVariantList getConstraintAsList();

    static QString separator();

private:
    QString _meta_name = "";
    QString _constrait = "";
};

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
    static void init( QString aim );

    template< typename Type >
    static void addConstraint( Type type, Constraint constr )
    {
        auto contaier = ConstraintsManager::containerInstance< Type >();
        contaier[ type ].push_back( constr );
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

private:
    template< typename Type >
    static std::map< Type, std::vector< Constraint >>& containerInstance()
    {
        static std::map< Type, std::vector< Constraint >> contaier;
        return contaier;
    }
};

}

#endif // RB_META_CONSTRAINTS_H
