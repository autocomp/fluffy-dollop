#ifndef RB_TRANSLATOR_H
#define RB_TRANSLATOR_H

#include <memory>
#include <QVariantMap>

namespace regionbiz {

class BaseTranslator;
typedef std::shared_ptr< BaseTranslator > BaseTranslatorPtr;

class BaseTranslator
{
public:
    bool init( QVariantMap settings );

    // get name
    virtual QString getTranslatorName() = 0;

    // convert
    template< typename Type >
    static std::shared_ptr< Type > convert( BaseTranslatorPtr ptr )
    {
        return std::dynamic_pointer_cast< Type >( ptr );
    }

protected:
    virtual void loadFunctions() = 0;
    virtual bool initBySettings(QVariantMap /*settings*/ );
};
typedef std::vector< BaseTranslatorPtr > BaseTranslatorPtrs;

//-----------------------------------------------------------------

class BaseTranslatorFabric
{
    template< typename Translator >
    friend class BaseTranslatorFabricRegister;

public:
    static BaseTranslatorPtr getTranslatorByName(QString &name );

private:
    static void addTranslator(BaseTranslatorPtr translator );
    static BaseTranslatorPtrs& getTranslators();
};

//------------------------------------------------------------------

/**
 * This macro must using ones per Traslator class
 * becase it's create global register object with class name.
 * It's involve header dependencys
 **/
#define REGISTER_TRANSLATOR( trans ) \
    BaseTranslatorFabricRegister< trans > base_register_ ## trans;

template< typename Translator >
class BaseTranslatorFabricRegister
{
public:
    BaseTranslatorFabricRegister()
    {
        BaseTranslatorPtr ptr = BaseTranslatorPtr( new Translator() );
        BaseTranslatorFabric::addTranslator( BaseTranslatorPtr( ptr ));
    }
};

}

#endif // RB_TRANSLATOR_H
