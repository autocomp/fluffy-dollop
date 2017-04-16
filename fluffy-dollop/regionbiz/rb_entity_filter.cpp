#include "rb_entity_filter.h"

#include <QRegExp>

#include "rb_manager.h"

using namespace regionbiz;

bool BaseFilterParam::isFiltered(BaseEntityPtr entity)
{
    // check type
    if( entity->getEntityType() != BaseEntity::ET_AREA )
        return true;

    BaseAreaPtr area = entity->convert< BaseArea >();
    if( !area )
        return false;

    // if room - check metadate
    if( BaseArea::AT_ROOM == area->getType() )
    {
        if( !entity->isMetadataPresent( _name ))
            return _get_empty_metadata;
        auto metadata = entity->getMetadata( _name );

        return isFilteredMetadate( metadata );
    }
    // if not - check all childs
    else
    {
        // WARNING optimize - return in parent filter
        //return true;

        BaseAreaPtrs childs = area->getChilds();
        bool filtered = childs.size();

        return filtered;
    }
}

void BaseFilterParam::setName(QString name)
{
    _name = name;
}

void BaseFilterParam::setGetEmptyMetadate(bool flag)
{
    _get_empty_metadata = flag;
}

bool DoubleFilterParam::isFilteredMetadate(BaseMetadataPtr metadata)
{
    if( "double" != metadata->getType() )
        return true;

    double value = metadata->getValueAsVariant().toDouble();
    bool filtered = true;
    if( _has_max )
        filtered = filtered && _max >= value;
    if( _has_min )
        filtered = filtered && _min <= value;

    return filtered;
}

QString DoubleFilterParam::toString()
{
    QString filter_str = _name;
    if( _has_min )
        filter_str += "," + QString::number( _min );
    if( _has_max )
        filter_str += "," + QString::number( _max );
    filter_str += ";";

    return filter_str;
}

void DoubleFilterParam::setMax(double max)
{
    _has_max = true;
    _max = max;
}

void DoubleFilterParam::setMin(double min)
{
    _has_min = true;
    _min = min;
}

bool StringFilterParam::isFilteredMetadate(BaseMetadataPtr metadata)
{
    if( "string" != metadata->getType() )
        return true;

    QString value = metadata->getValueAsVariant().toString();
    bool filtered = true;
    if( _check )
    {
        QRegExp rx( _reg_exp );
        rx.setPatternSyntax(QRegExp::Wildcard);
        filtered = filtered && rx.exactMatch( value );
    }

    return filtered;
}

QString StringFilterParam::toString()
{
    QString filter_str = _name;
    if( _check )
        filter_str += "," + _reg_exp;
    filter_str += ";";

    return filter_str;
}

void StringFilterParam::setRegExp(QString exp)
{
    _check = true;
    _reg_exp = exp;
}

bool EntityFilter::addFilter(BaseFilterParamPtr filter)
{
    // check already have
    for( BaseFilterParamPtr filter: instance()._params )
        if( filter->toString() == filter->toString() )
            return false;

    instance()._params.push_back( filter );
    instance().emitFilterChanged();

    return true;
}

bool EntityFilter::setFilters(BaseFilterParamPtrs filters)
{
    // check equal
    QString filter_str = "";
    for( BaseFilterParamPtr filter: filters )
        filter_str += filter->toString();
    if( filter_str == instance().toString() )
        return false;

    // set new filters
    instance()._params = filters;
    instance().emitFilterChanged();

    return true;
}

void EntityFilter::clearFilters()
{
    setFilters( BaseFilterParamPtrs() );
}

bool EntityFilter::isFiltered( uint64_t id )
{
    auto entity = RegionBizManager::instance()->getBaseEntity( id );
    return isFiltered( entity );
}

bool EntityFilter::isFiltered(BaseEntityPtr entity)
{
    if( !entity )
        return false;

    bool filtered = true;
    for( BaseFilterParamPtr param: instance()._params )
        filtered = filtered && param->isFiltered( entity );

    return filtered;
}

void EntityFilter::subscribeOnFilterChacnge( QObject *obj,
                                             const char *slot,
                                             bool queue )
{
    QObject::connect( &(instance()), SIGNAL( filterChanged() ),
                      obj, slot, ( queue ? Qt::QueuedConnection : Qt::DirectConnection ));
}

EntityFilter& EntityFilter::instance()
{
    static EntityFilter filter;
    return filter;
}

void EntityFilter::emitFilterChanged()
{
    auto mngr = RegionBizManager::instance();
    mngr->clearSelect();

    Q_EMIT filterChanged();
}

QString EntityFilter::toString()
{
    QString filter_str = "";
    for( BaseFilterParamPtr filter: _params )
        filter_str += filter->toString();

    return filter_str;
}
