#include "rb_files.h"

#include "rb_manager.h"

using namespace regionbiz;

BaseFileKeeper::BaseFileKeeper(QString path, uint64_t entity_id):
    _path( path ),
    _entity_id( entity_id )
{}

QString BaseFileKeeper::getPath()
{
    return _path;
}

uint64_t BaseFileKeeper::getEntityId()
{
    return _entity_id;
}

void BaseFileKeeper::setName(QString name)
{
    _name = name;
}

QString BaseFileKeeper::getName()
{
    return _name;
}

QFilePtr BaseFileKeeper::getLocalFile()
{
    auto mngr = RegionBizManager::instance();
    auto qfile = mngr->getLocalFile( getItself() );
    return qfile;
}

BaseFileKeeper::FileState BaseFileKeeper::getFileState()
{
    auto mngr = RegionBizManager::instance();
    auto state = mngr->getFileState( getItself() );
    return state;
}

BaseFileKeeper::FileState BaseFileKeeper::syncFile()
{
    auto mngr = RegionBizManager::instance();
    auto state = mngr->syncFile( getItself() );
    return state;
}

bool BaseFileKeeper::commit()
{
    auto mngr = RegionBizManager::instance();
    return mngr->commitFile( getItself() );
}

FileKeepersById &BaseFileKeeper::getFiles()
{
    static FileKeepersById files;
    return files;
}

std::recursive_mutex &BaseFileKeeper::getMutex()
{
    static std::recursive_mutex mutex;
    return mutex;
}

BaseFileKeeperPtr BaseFileKeeper::getItself()
{
    BaseFileKeeperPtr itself = nullptr;

    BaseFileKeeper::getMutex().lock();
    auto&& files = getFiles()[ _entity_id ];
    for( BaseFileKeeperPtr ptr: files )
        if( ptr->getPath() == _path )
            itself = ptr ;
    BaseFileKeeper::getMutex().unlock();

    return itself;
}

//--------------------------------------------------

BaseFileKeeperPtr FileKeeperFabric::createFile(QString path, uint64_t entity_id,
                                               BaseFileKeeper::FileType type)
{
    switch( type )
    {
    case BaseFileKeeper::FT_PLAN:
        return BaseFileKeeperPtr( new PlanFileKeeper( path, entity_id ));
    case BaseFileKeeper::FT_DOCUMENT:
        return BaseFileKeeperPtr( new DocumentFileKeeper( path, entity_id ));
    case BaseFileKeeper::FT_IMAGE:
        return BaseFileKeeperPtr( new ImageFileKeeper( path, entity_id ));
    case BaseFileKeeper::FT_NONE:
        return nullptr;
    }

    return nullptr;
}

//----------------------------------------------------

PlanFileKeeper::PlanFileKeeper(QString path, uint64_t entity_id ):
    BaseFileKeeper( path, entity_id )
{}

BaseFileKeeper::FileType PlanFileKeeper::getType()
{
    return FT_PLAN;
}

PlanFileKeeper::PlanParams PlanFileKeeper::getPlanParams()
{
    return _params;
}

void PlanFileKeeper::setPlanParams(PlanParams params)
{
    _params = params;
}

//------------------------------------------------------

DocumentFileKeeper::DocumentFileKeeper(QString path, uint64_t entity_id):
    BaseFileKeeper( path, entity_id )
{}

BaseFileKeeper::FileType DocumentFileKeeper::getType()
{
    return FT_DOCUMENT;
}

//------------------------------------------------------

ImageFileKeeper::ImageFileKeeper(QString path, uint64_t entity_id):
    BaseFileKeeper( path, entity_id )
{}

BaseFileKeeper::FileType ImageFileKeeper::getType()
{
    return FT_IMAGE;
}
