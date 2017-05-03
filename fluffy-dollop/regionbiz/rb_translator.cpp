#include "rb_translator.h"

#include <iostream>

using namespace regionbiz;

bool BaseTranslator::init(QVariantMap settings)
{
    loadFunctions();
    bool init_correct = initBySettings( settings );
    return init_correct;
}

bool BaseTranslator::initBySettings(QVariantMap)
{
    return true;
}

//--------------------------------------------------------------

BaseTranslatorPtr BaseTranslatorFabric::getTranslatorByName( QString &name )
{
    for( BaseTranslatorPtr tranlator: getTranslators() )
        if( tranlator->getTranslatorName() == name )
            return tranlator;

    return nullptr;
}

void BaseTranslatorFabric::addTranslator( BaseTranslatorPtr translator )
{
    std::cout << "Add translator: "
              << translator->getTranslatorName().toUtf8().data() << std::endl;
    getTranslators().push_back( translator );
}

BaseTranslatorPtrs &BaseTranslatorFabric::getTranslators()
{
    static BaseTranslatorPtrs translarots;
    return translarots;
}
