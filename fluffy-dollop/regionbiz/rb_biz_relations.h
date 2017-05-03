#ifndef RB_BIZ_RELATION_H
#define RB_BIZ_RELATION_H

#include <memory>
#include <QDateTime>
#include <QString>
#include <vector>

namespace regionbiz {

class BizDocument
{
public:

private:
    QString _type;
    QDate _date;
    QString _file_path;
};
typedef std::shared_ptr< BizDocument > BizDocumentPtr;
typedef std::vector< BizDocumentPtr > BizDocumentPtrs;

//---------------------------------------

class BaseBizRelation;
typedef std::shared_ptr< BaseBizRelation > BaseBizRelationPtr;

class BaseBizRelation
{
    friend class BaseDataTranslator;

public:
    enum RelationType
    {
        RT_PROPERTY,
        RT_RENT
    };

    // main
    virtual RelationType getType() = 0;

    // params
    uint64_t getId();
    uint64_t getAreaId();
    BizDocumentPtrs getDocuments();
    void addDocument( BizDocumentPtr doc );

    template< typename Type >
    static std::shared_ptr< Type > convert( BaseBizRelationPtr base )
    {
        return std::dynamic_pointer_cast< Type >( base );
    }

protected:
    BaseBizRelation( uint64_t id );

    uint64_t _area_id;
    uint64_t _id;
    BizDocumentPtrs _documents;

private:
    void setAreaId( uint64_t id );
};
typedef std::vector< BaseBizRelationPtr > BaseBizRelationPtrs;

//-----------------------------------------

class Property: public BaseBizRelation
{
public:
    // main
    Property( uint64_t id );
    RelationType getType() override;

    // params
    QDate getDateOfRegistration();
    void setDateOfRegistration( QDate date );
    const QStringList& getEncumbrances();
    void addEncumbrance( QString enc );

private:
    QDate _date_registration;
    // WARNING type of encumbrances
    QStringList _encumbrances;
};
typedef std::shared_ptr< Property > PropertyPtr;
typedef std::vector< PropertyPtr > PropertyPtrs;

//-----------------------------------------

class Rent: public BaseBizRelation
{
public:
    struct Payment
    {
        QDate date;
        int16_t money;
    };

    // main
    Rent( uint64_t id );
    RelationType getType() override;

    // params
    QDate getDateOfStart();
    void setDateOfStart( QDate date );
    QDate getDateOfFinish();
    void setDateOfFinish( QDate date );
    std::vector< Payment > getPayments();
    void addPayment( Payment pay );
    std::vector< Payment > getPayShedule();
    void addToPayShedule( Payment pay );

private:
    QDate _date_start;
    QDate _date_end;
    std::vector< Payment > _payments;
    std::vector< Payment > _pay_shedule;
};
typedef std::shared_ptr< Rent > RentPtr;
typedef std::vector< RentPtr > RentPtrs;

//-------------------------------------------

class BaseBizExecutor
{

};

//-------------------------------------------

class Man: public BaseBizExecutor
{

};

//-------------------------------------------

class BaseBizPerson
{
public:
    enum BizPersonType
    {
        BPT_INDIVIDUAL,
        BPT_ORGANIZATION
    };

protected:
    uint64_t _id;
};

//-------------------------------------------

class Organization: public BaseBizPerson, public BaseBizExecutor
{
private:    
        
};

//-------------------------------------------

class Individual: public BaseBizPerson, public Man
{

};

//-------------------------------------------

class Employee
{

};

}

#endif // RB_BIZ_RELATION_H
