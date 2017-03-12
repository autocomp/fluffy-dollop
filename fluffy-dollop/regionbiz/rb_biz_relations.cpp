#include "rb_biz_relations.h"

using namespace regionbiz;

uint64_t BaseBizRelation::getId()
{
    return _id;
}

uint64_t BaseBizRelation::getAreaId()
{
    return _area_id;
}

BizDocumentPtrs BaseBizRelation::getDocuments()
{
    return _documents;
}

void BaseBizRelation::addDocument(BizDocumentPtr doc)
{
    _documents.push_back( doc );
}

BaseBizRelation::BaseBizRelation(uint64_t id):
    _id( id )
{}

void BaseBizRelation::setAreaId(uint64_t id)
{
    _area_id = id;
}

Property::Property(uint64_t id):
    BaseBizRelation( id )
{}

BaseBizRelation::RelationType Property::getType()
{
    return RT_PROPERTY;
}

QDate Property::getDateOfRegistration()
{
    return _date_registration;
}

void Property::setDateOfRegistration(QDate date)
{
    _date_registration = date;
}

const QStringList &Property::getEncumbrances()
{
    const QStringList& link = _encumbrances;
    return link;
}

void Property::addEncumbrance(QString enc)
{
    _encumbrances.append( enc );
}

Rent::Rent(uint64_t id):
    BaseBizRelation( id )
{}

BaseBizRelation::RelationType Rent::getType()
{
    return RT_RENT;
}

QDate Rent::getDateOfStart()
{
    return _date_start;
}

void Rent::setDateOfStart( QDate date )
{
    _date_start = date;
}

QDate Rent::getDateOfFinish()
{
    return _date_end;
}

void Rent::setDateOfFinish(QDate date)
{
    _date_end = date;
}

std::vector<Rent::Payment> Rent::getPayments()
{
    return _payments;
}

void Rent::addPayment(Rent::Payment pay)
{
    _payments.push_back( pay );
}

std::vector<Rent::Payment> Rent::getPayShedule()
{
    return _pay_shedule;
}

void Rent::addToPayShedule(Rent::Payment pay)
{
    _pay_shedule.push_back( pay );
}
