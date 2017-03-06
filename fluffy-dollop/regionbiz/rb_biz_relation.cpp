#include "rb_biz_relation.h"

using namespace regionbiz;

BaseBizRelation::BaseBizRelation(uint64_t id):
    _id( id )
{}

Property::Property(uint64_t id):
    BaseBizRelation( id )
{}

Rent::Rent(uint64_t id):
    BaseBizRelation( id )
{}
