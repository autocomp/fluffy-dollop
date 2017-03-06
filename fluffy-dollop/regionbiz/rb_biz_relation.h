#ifndef RB_BIZ_RELATION_H
#define RB_BIZ_RELATION_H

#include <memory>
#include <QDateTime>
#include <QString>

namespace regionbiz {

class BizDocument
{
public:

private:
    QString _type;
    QDateTime _date;
    QString _file_path;
};

//---------------------------------------

class BaseBizRelation
{
protected:
    BaseBizRelation( uint64_t id );

    uint64_t _area_id;
    uint64_t _id;
    std::vector< BizDocument > _documents;
};
typedef std::shared_ptr< BaseBizRelation > BaseBizRelationPtr;

//-----------------------------------------

class Property: public BaseBizRelation
{
public:
    Property( uint64_t id );

private:
    QDateTime _date_registration;
    // WARNING type of encumbrances
    QStringList _encumbrances;
};
typedef std::shared_ptr< Property > PropertyPtr;

//-----------------------------------------

class Rent: public BaseBizRelation
{
public:
    Rent( uint64_t id );

private:
    QDateTime _date_start;
    QDateTime _date_end;
    // WARNING type of payments
    QStringList _payments;
};

//-------------------------------------------

// TODO relation between Organization and Individual
class BaseBizPerson
{

};

}

#endif // RB_BIZ_RELATION_H
