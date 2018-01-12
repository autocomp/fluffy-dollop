#ifndef RB_METADATA_H
#define RB_METADATA_H

#include <memory>
#include <map>
#include <unordered_map>
#include <mutex>
#include <QString>
#include <QVariant>

#include "rb_meta_constraints.h"

namespace regionbiz {

class Layer;
typedef std::shared_ptr< Layer > LayerPtr;

class BaseMetadata;
typedef std::shared_ptr< BaseMetadata > BaseMetadataPtr;
typedef std::map< QString, BaseMetadataPtr > MetadataByName;
typedef std::unordered_map< uint64_t, MetadataByName > MetadataById;

class BaseMetadata
{
    friend class RegionBizManager;
    friend class BaseDataTranslator;

public:
    BaseMetadata( uint64_t parent_id );

    // type
    virtual QString getType() = 0;

    // constraits
    virtual bool checkConstraits() = 0;
    virtual bool checkConstraitsByVariant( QVariant new_value ) = 0;

    // value
    virtual QString getValueAsString() = 0;
    virtual bool setValueByString( QString val ) = 0;
    virtual QVariant getValueAsVariant() = 0;
    virtual bool setValueByVariant( QVariant val ) = 0;
    virtual bool isEmpty();

    // getters
    uint64_t getParentId();
    QString getName();
    void setName( QString name );

    // layers
    LayerPtr getLayer();
    void moveToLayer( LayerPtr layer );
    void leaveLayer();

    // convert
    template< typename Type >
    static std::shared_ptr< Type > convert( BaseMetadataPtr base )
    {
        return std::dynamic_pointer_cast< Type >( base );
    }
    template< typename Type >
    std::shared_ptr< Type > convert()
    {
        return convert< Type >( getItself() );
    }

protected:
    std::vector< Constraint > getConstraints();
    void printIncorrectConstraint( Constraint cons );
    void printWrongCheckConstraints();
    BaseMetadataPtr getItself();

    uint64_t _parent_id;
    QString _name;

private:
    static void removeForEntity( uint64_t id );
    static void addForEntityByName( BaseMetadataPtr data );
    static MetadataById& getMetadatas();
    static std::recursive_mutex& getMutex();
};
typedef std::vector< BaseMetadataPtr > BaseMetadataPtrs;

//-----------------------------------------------

class DoubleMetadata: public BaseMetadata
{
public:
    DoubleMetadata( uint64_t parent_id );

    // type
    QString getType() override;

    // constraits
    bool checkConstraits() override;
    bool checkConstraitsByVariant( QVariant new_value ) override;

    // value
    QString getValueAsString() override;
    bool setValueByString( QString val ) override;
    QVariant getValueAsVariant() override;
    bool setValueByVariant( QVariant val );
    double getValue();
    bool setValue( double val );

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

    // constraits
    bool checkConstraits() override;
    bool checkConstraitsByVariant( QVariant new_value ) override;

    // value
    QString getValueAsString() override;
    bool setValueByString( QString val ) override;
    QVariant getValueAsVariant() override;
    bool setValueByVariant( QVariant val );
    QString getValue();
    bool setValue(QString val );

private:
    QString _value;
};
typedef std::shared_ptr< StringMetadata > StringMetadataPtr;
typedef std::vector< StringMetadataPtr > StringMetadataPtrs;

//-----------------------------------------

class EnumMetadata: public BaseMetadata
{
public:
    EnumMetadata( uint64_t parent_id );

    // type
    QString getType() override;

    // constraits
    bool checkConstraits() override;
    bool checkConstraitsByVariant( QVariant new_value ) override;

    // value
    QString getValueAsString() override;
    bool setValueByString( QString val ) override;
    QVariant getValueAsVariant() override;
    bool setValueByVariant( QVariant val );

    QString getValue();
    bool setValue( QString val );
    int getNumberOfValue();

private:
    QString _value;
};
typedef std::shared_ptr< EnumMetadata > EnumMetadataPtr;
typedef std::vector< EnumMetadataPtr > EnumMetadataPtrs;

//--------------------------------------------------

class IntegerMetadata: public BaseMetadata
{
public:
    IntegerMetadata( uint64_t parent_id );

    // type
    QString getType() override;

    // constraits
    bool checkConstraits() override;
    bool checkConstraitsByVariant( QVariant new_value ) override;

    // value
    QString getValueAsString() override;
    bool setValueByString( QString val ) override;
    QVariant getValueAsVariant() override;
    bool setValueByVariant( QVariant val );
    int getValue();
    bool setValue( int val );

private:
    int _value;
};
typedef std::shared_ptr< IntegerMetadata > IntegerMetadataPtr;
typedef std::vector< IntegerMetadataPtr > IntegerMetadataPtrs;

//--------------------------------------------------

class EmptyMetadata;
typedef std::shared_ptr< EmptyMetadata > EmptyMetadataPtr;

class EmptyMetadata: public BaseMetadata
{
public:
    static EmptyMetadataPtr instance();

    // type
    QString getType() override;

    // constraits
    bool checkConstraits() override;
    bool checkConstraitsByVariant( QVariant new_value ) override;

    // value
    QString getValueAsString() override;
    bool setValueByString(QString) override;
    QVariant getValueAsVariant() override;
    bool setValueByVariant( QVariant val ) override;
    bool isEmpty() override;

private:
    EmptyMetadata();
};

//--------------------------------------------------

class MetadataFabric
{
public:
    static BaseMetadataPtr createMetadata( QString type,
                                           uint64_t parent_id );
};

}

#endif // RB_METADATA_H
