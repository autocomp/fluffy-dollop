#ifndef RB_ENTITY_FILTER_H
#define RB_ENTITY_FILTER_H

#include <QObject>
#include <memory>

#include "rb_base_entity.h"
#include "rb_locations.h"

namespace regionbiz {

// TODO make base filter param class more universal
class BaseFilterParam
{
public:
    BaseFilterParam(){}
    bool isFiltered( BaseEntityPtr entity );
    void setName( QString name );
    void setGetEmptyMetadate( bool flag );

    virtual QString toString() = 0;

protected:
    virtual bool isFilteredMetadate( BaseMetadataPtr metadata ) = 0;

    QString _name = "";
    bool _get_empty_metadata = false;
};
typedef std::shared_ptr< BaseFilterParam > BaseFilterParamPtr;
typedef std::vector< BaseFilterParamPtr > BaseFilterParamPtrs;

class DoubleFilterParam: public BaseFilterParam
{
public:
    DoubleFilterParam(){}

    bool isFilteredMetadate( BaseMetadataPtr metadata );
    QString toString();
    void setMax( double max );
    void setMin( double min );

private:
    bool _has_max = false;
    double _max = 0;
    bool _has_min = false;
    double _min = 0;
};
typedef std::shared_ptr< DoubleFilterParam > DoubleFilterParamPtr;

class StringFilterParam: public BaseFilterParam
{
public:
    StringFilterParam(){}

    bool isFilteredMetadate( BaseMetadataPtr metadata );
    QString toString();
    void setRegExp( QString exp );

private:
    bool _check = false;
    QString _reg_exp = "*";
};
typedef std::shared_ptr< StringFilterParam > StringFilterParamPtr;

class EntityFilter: public QObject
{
    Q_OBJECT
public:
    static bool addFilter( BaseFilterParamPtr filter );
    static bool setFilters( BaseFilterParamPtrs filters );
    static void clearFilters();

    static bool isFiltered( uint64_t id );
    static bool isFiltered( BaseEntityPtr entity );

    //! if filter changed. Signature:
    //! slot()
    static void subscribeOnFilterChacnge( QObject* obj,
                                          const char *slot,
                                          bool queue = false );

Q_SIGNALS:
    void filterChanged();

private:
    static EntityFilter& instance();
    void emitFilterChanged();
    QString toString();

    BaseFilterParamPtrs _params;
};

class BaseFilterParamFabric
{
public:
    template< typename Type >
    static std::shared_ptr< Type > createFilter( QString name = "" )
    {
        auto filter = std::shared_ptr< Type >( new Type() );
        filter->setName( name );
        return filter;
    }
};

}

#endif // RB_ENTITY_FILTER_H
