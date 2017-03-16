#ifndef RB_METADATA_H
#define RB_METADATA_H

#include <memory>
#include <map>
#include <QString>
#include <QVariant>

namespace regionbiz {


class BaseMetadata;
typedef std::shared_ptr< BaseMetadata > BaseMetadataPtr;
typedef std::map< QString, BaseMetadataPtr > MetadataByName;
typedef std::map< uint64_t, MetadataByName > MetadataById;

class BaseMetadata
{
public:
    BaseMetadata( uint64_t parent_id );

    // type
    virtual QString getType() = 0;

    // value
    virtual QString getValueAsString() = 0;
    virtual void setValueByString( QString val ) = 0;
    virtual QVariant getValueAsVariant() = 0;
    virtual void setValueByVariant( QVariant val ) = 0;

    // getters
    uint64_t getParentId();
    QString getName();
    void setName( QString name );

    // convert
    template< typename Type >
    static std::shared_ptr< Type > convert( BaseMetadataPtr base )
    {
        return std::dynamic_pointer_cast< Type >( base );
    }

protected:
    uint64_t _parent_id;
    QString _name;
};
typedef std::vector< BaseMetadataPtr > BaseMetadataPtrs;

//-----------------------------------------------

class DoubleMetadata: public BaseMetadata
{
public:
    DoubleMetadata( uint64_t parent_id );

    // type
    QString getType() override;

    // value
    QString getValueAsString() override;
    void setValueByString( QString val ) override;
    QVariant getValueAsVariant() override;
    void setValueByVariant( QVariant val );
    double getValue();
    void setValue( double val );

private:
    double _value;
};
typedef std::shared_ptr< DoubleMetadata > DoubleMetadataPtr;
typedef std::vector< DoubleMetadataPtr > DoubleMetadataPtrs;

//-------------------------------------------------

class StringMetadata: public BaseMetadata
{
public:
    StringMetadata( uint64_t parent_id );

    // type
    QString getType() override;

    // value
    QString getValueAsString() override;
    void setValueByString( QString val ) override;
    QVariant getValueAsVariant() override;
    void setValueByVariant( QVariant val );
    QString getValue();
    void setValue(QString val );

private:
    QString _value;
};
typedef std::shared_ptr< StringMetadata > StringMetadataPtr;
typedef std::vector< StringMetadataPtr > StringMetadataPtrs;

//--------------------------------------------------

class MetadataFabric
{
public:
    static BaseMetadataPtr createMetadata( QString type,
                                           uint64_t parent_id );
};

}

#endif // RB_METADATA_H
