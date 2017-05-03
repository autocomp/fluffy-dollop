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

FileKeepersById &BaseFileKeeper::getFiles()
{
    static FileKeepersById files;
    return files;
}

BaseFileKeeperPtr BaseFileKeeper::getItself()
{
    auto&& files = getFiles()[ _entity_id ];
    for( BaseFileKeeperPtr ptr: files )
        if( ptr->getPath() == _path )
            return ptr;
    return nullptr;
}

//--------------------------------------------------

BaseFileKeeperPtr FileKeeperFabric::createFile(QString path, uint64_t entity_id,
                                               BaseFileKeeper::FileType type)
{
    switch( type )
    {
    case BaseFileKeeper::FT_PLAN:
        return BaseFileKeeperPtr( new PlanFileKeeper( path, entity_id ));

    }
}

//----------------------------------------------------

PlanFileKeeper::PlanFileKeeper(QString path, uint64_t entity_id ):
    BaseFileKeeper( path, entity_id )
{}

BaseFileKeeper::FileType PlanFileKeeper::getType()
{
    return BaseFileKeeper::FT_PLAN;
}

PlanFileKeeper::PlanParams PlanFileKeeper::getPlanParams()
{
    return _params;
}

void PlanFileKeeper::setPlanParams(PlanParams params)
{
    _params = params;
}
