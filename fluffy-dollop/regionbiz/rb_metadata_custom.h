#ifndef RB_METADATA_CUSTOM_H
#define RB_METADATA_CUSTOM_H

#include <memory>

#include "rb_metadata.h"

namespace regionbiz {

class BaseCustomMetadata: public BaseMetadata
{
public:
    BaseCustomMetadata( uint64_t parent_id );

    /* type */
    virtual QString getType() = 0;

    /* constraits */
    bool checkConstraits() override;
    bool checkConstraitsByVariant( QVariant new_value ) override;

    /* value */
    QString getValueAsString() override;
    bool setValueByString( QString val ) override;
    bool setValueByVariant( QVariant val );
    virtual QVariant getValueAsVariant() = 0;
};

//--------------------------------------------

#define CUSTOM_METADATA_CLASS( metatype ) \
class metatype: public BaseCustomMetadata \
{ \
public: \
    metatype( uint64_t parent_id ); \
    QString getType() override; \
    QVariant getValueAsVariant() override; \
}; \
typedef std::shared_ptr< metatype > metatype ## Ptr

CUSTOM_METADATA_CLASS( MetadataSum );
CUSTOM_METADATA_CLASS( MetadataMin );
CUSTOM_METADATA_CLASS( MetadataMax );
CUSTOM_METADATA_CLASS( MetadataAverage );

#undef CUSTOM_METADATA_CLASS

}

#endif // RB_METADATA_CUSTOM_H
