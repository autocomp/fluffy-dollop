#include "rb_metadata_custom.h"

#include <iostream>

#include <regionbiz/rb_manager.h>

using namespace regionbiz;

BaseCustomMetadata::BaseCustomMetadata(uint64_t parent_id):
    BaseMetadata( parent_id )
{}

bool BaseCustomMetadata::checkConstraits()
{
    return true;
}

bool BaseCustomMetadata::checkConstraitsByVariant( QVariant /*new_value*/ )
{
    return true;
}

QString BaseCustomMetadata::getValueAsString()
{
    return getValueAsVariant().toString();
}

bool BaseCustomMetadata::setValueByString( QString val )
{
    return setValueByVariant( val );
}

bool BaseCustomMetadata::setValueByVariant(QVariant /*val*/)
{
    return true;
}

//-----------------------------------------

#define CUSTOM_METADATA_REALISATION( name, type ) \
type::type(uint64_t parent_id): \
    BaseCustomMetadata( parent_id ) \
{} \
QString type::getType() \
{ \
    return name; \
}

//-----------------------------------------

CUSTOM_METADATA_REALISATION( "sum", MetadataSum )
QVariant MetadataSum::getValueAsVariant()
{
    auto mngr = RegionBizManager::instance();
    auto entity = mngr->getBaseEntity( _parent_id );
    if( entity )
    {
        switch( entity->getEntityType() ) {
        case BaseEntity::ET_GROUP:
        {
            double res = 0;

            auto childs = entity->convert< GroupEntity >()->getElements();
            for( BaseAreaPtr area: childs )
            {
                if( area->isMetadataPresent( getName() ))
                    res += area->getMetadataValue( getName() ).toDouble();
            }

            return res;
        }

        case BaseEntity::ET_AREA:
        {
            double res = 0;

            auto childs = entity->convert< BaseArea >()->getBaseAreaChilds();
            //BaseAreaPtrs childs;
            for( BaseAreaPtr area: childs )
            {
                if( area->isMetadataPresent( getName() ))
                    res += area->getMetadataValue( getName() ).toDouble();
            }

            auto marks = entity->convert< BaseArea >()->convertToMarksHolder()->getMarks();
            //MarkPtrs marks;
            for( MarkPtr mark: marks )
            {
                if( mark->isMetadataPresent( getName() ))
                    res += mark->getMetadataValue( getName() ).toDouble();
            }

            return res;
        }

        case BaseEntity::ET_MARK:
        case BaseEntity::ET_RELATION:
        {
            break;
        }

        }
    }

    return QVariant();
}

CUSTOM_METADATA_REALISATION( "min", MetadataMin )
QVariant MetadataMin::getValueAsVariant()
{
    auto mngr = RegionBizManager::instance();
    auto entity = mngr->getBaseEntity( _parent_id );
    if( entity )
    {
        switch( entity->getEntityType() ) {
        case BaseEntity::ET_GROUP:
        {
            QVariant res = QVariant();

            auto childs = entity->convert< GroupEntity >()->getElements();
            for( BaseAreaPtr area: childs )
            {
                if( area->isMetadataPresent( getName() ))
                {
                    QVariant next_val = area->getMetadataValue( getName() );
                    if( !res.isValid()
                            || next_val < res )
                        res = next_val;
                }
            }

            return res;
        }

        case BaseEntity::ET_AREA:
        {
            QVariant res = QVariant();

            auto childs = entity->convert< BaseArea >()->getBaseAreaChilds();
            for( BaseAreaPtr area: childs )
            {
                if( area->isMetadataPresent( getName() ))
                {
                    QVariant next_val = area->getMetadataValue( getName() );
                    if( !res.isValid()
                            || next_val < res )
                        res = next_val;
                }
            }

            auto marks = entity->convert< BaseArea >()->convertToMarksHolder()->getMarks();
            for( MarkPtr mark: marks )
            {
                if( mark->isMetadataPresent( getName() ))
                {
                    QVariant next_val = mark->getMetadataValue( getName() );
                    if( !res.isValid()
                            || next_val < res )
                        res = next_val;
                }
            }

            return res;
        }

        case BaseEntity::ET_MARK:
        case BaseEntity::ET_RELATION:
        {
            break;
        }

        }
    }

    return QVariant();
}

CUSTOM_METADATA_REALISATION( "max", MetadataMax )
QVariant MetadataMax::getValueAsVariant()
{
    auto mngr = RegionBizManager::instance();
    auto entity = mngr->getBaseEntity( _parent_id );
    if( entity )
    {
        switch( entity->getEntityType() ) {
        case BaseEntity::ET_GROUP:
        {
            QVariant res = QVariant();

            auto childs = entity->convert< GroupEntity >()->getElements();
            for( BaseAreaPtr area: childs )
            {
                if( area->isMetadataPresent( getName() ))
                {
                    QVariant next_val = area->getMetadataValue( getName() );
                    if( !res.isValid()
                            || next_val > res )
                        res = next_val;
                }
            }

            return res;
        }

        case BaseEntity::ET_AREA:
        {
            QVariant res = QVariant();

            auto childs = entity->convert< BaseArea >()->getBaseAreaChilds();
            for( BaseAreaPtr area: childs )
            {
                if( area->isMetadataPresent( getName() ))
                {
                    QVariant next_val = area->getMetadataValue( getName() );
                    if( !res.isValid()
                            || next_val > res )
                        res = next_val;
                }
            }

            auto marks = entity->convert< BaseArea >()->convertToMarksHolder()->getMarks();
            for( MarkPtr mark: marks )
            {
                if( mark->isMetadataPresent( getName() ))
                {
                    QVariant next_val = mark->getMetadataValue( getName() );
                    if( !res.isValid()
                            || next_val > res )
                        res = next_val;
                }
            }

            return res;
        }

        case BaseEntity::ET_MARK:
        case BaseEntity::ET_RELATION:
        {
            break;
        }

        }
    }

    return QVariant();
}

CUSTOM_METADATA_REALISATION( "average", MetadataAverage )
QVariant MetadataAverage::getValueAsVariant()
{
    auto mngr = RegionBizManager::instance();
    auto entity = mngr->getBaseEntity( _parent_id );
    if( entity )
    {
        switch( entity->getEntityType() ) {
        case BaseEntity::ET_GROUP:
        {
            double res = 0;
            uint count = 0;

            auto childs = entity->convert< GroupEntity >()->getElements();
            for( BaseAreaPtr area: childs )
            {
                if( area->isMetadataPresent( getName() ))
                {
                    res += area->getMetadataValue( getName() ).toDouble();
                    ++count;
                }
            }

            return ( count ? res / count : 0 );
        }

        case BaseEntity::ET_AREA:
        {
            double res = 0;
            uint count = 0;

            auto childs = entity->convert< BaseArea >()->getBaseAreaChilds();
            for( BaseAreaPtr area: childs )
            {
                if( area->isMetadataPresent( getName() ))
                {
                    res += area->getMetadataValue( getName() ).toDouble();
                    ++count;
                }
            }

            auto marks = entity->convert< BaseArea >()->convertToMarksHolder()->getMarks();
            for( MarkPtr mark: marks )
            {
                if( mark->isMetadataPresent( getName() ))
                {
                    res += mark->getMetadataValue( getName() ).toDouble();
                    ++count;
                }
            }

            return ( count ? res / count : 0 );
        }

        case BaseEntity::ET_MARK:
        case BaseEntity::ET_RELATION:
        {
            break;
        }

        }
    }

    return QVariant();
}

#undef CUSTOM_METADATA_REALISATION
