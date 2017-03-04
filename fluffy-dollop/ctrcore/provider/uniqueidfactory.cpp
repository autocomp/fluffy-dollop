#include "uniqueidfactory.h"

UniqueIdFactory *UniqueIdFactory::m_Instance = 0;

UniqueIdFactory::UniqueIdFactory()
    : idQueryCounter(0), listenerCounter(0) {}

UniqueIdFactory::~UniqueIdFactory()
{
    if(m_Instance)
    {
        delete m_Instance;
    }
}

unsigned int UniqueIdFactory::getIdQuery()
{
    if((++idQueryCounter) <= 0)
    {
        idQueryCounter = 1;
    }

    return idQueryCounter;
}

int UniqueIdFactory::getListener()
{
    if((++listenerCounter) <= 0)
    {
        listenerCounter = 1;
    }

    return listenerCounter;
}

