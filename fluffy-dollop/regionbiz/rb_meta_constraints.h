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
    static void init( QString aim );

    template< typename Type >
    static void addConstraint( Type type, Constraint constr );
    template< typename Type >
    static std::vector< Constraint > getConstraints( Type type );

private:
    template< typename Type >
    static std::map< Type, std::vector< Constraint >>& containerInstance();
};

}

#endif // RB_META_CONSTRAINTS_H
