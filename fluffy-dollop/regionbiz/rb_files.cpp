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

QString BaseFileKeeper::getId()
{
    QRegExp rx( "\\{.*\\}" );
    if( rx.indexIn( _path ) != -1 )
        return rx.cap( 0 );
    return "";
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

LayerPtr BaseFileKeeper::getLayer()
{
    auto mngr = RegionBizManager::instance();
    return mngr->getLayerOfFile( getPath() );
}

void BaseFileKeeper::moveToLayer(LayerPtr layer)
{
    if( layer )
        layer->addFile( getPath() );
}

void BaseFileKeeper::leaveLayer()
{
    auto layer = getLayer();
    if( layer )
        layer->removeFile( getPath() );
}

void BaseFileKeeper::addFile( BaseFileKeeperPtr file )
{
    addFile( file, file->getEntityId() );
}

void BaseFileKeeper::addFile(BaseFileKeeperPtr file, uint64_t entity_id)
{
    getMutex().lock();
    {
        FileKeepersById& files = BaseFileKeeper::getFiles();
        files[ entity_id ].push_back( file );

        getFilesByPath()[ file->getPath().toStdString() ] = file;
    }
    getMutex().unlock();
}

void BaseFileKeeper::deleteFile( BaseFileKeeperPtr file )
{
    getMutex().lock();
    {
        // by entity id
        auto check_path = [ file ]( BaseFileKeeperPtr our_file ){
            return our_file->getPath() == file->getPath();
        };

        auto iter = FIND_IF( getFiles()[ file->getEntityId() ], check_path );
        if( iter != getFiles()[ file->getEntityId() ].end() )
            getFiles()[ file->getEntityId() ].erase( iter );

        // by path
        auto iter_by_path = getFilesByPath().find( file->getPath().toStdString() );
        if( iter_by_path != getFilesByPath().end() )
            getFilesByPath().erase( iter_by_path );
    }
    getMutex().unlock();
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

BaseFileKeeperPtrs BaseFileKeeper::getFileKeepersByEntity(uint64_t entity_id)
{
    BaseFileKeeperPtrs files_keepers;

    getMutex().lock();
    {
        auto& files = BaseFileKeeper::getFiles();
        if( files.find( entity_id ) != files.end() )
            files_keepers = files[ entity_id ];
    }
    getMutex().unlock();

    return files_keepers;
}

BaseFileKeeperPtr BaseFileKeeper::getFileByPath( QString path )
{
    BaseFileKeeperPtr file;

    getMutex().lock();
    {
        auto iter = getFilesByPath().find( path.toStdString() );
        if( iter != getFilesByPath().end() )
            file = (*iter).second;
    }
    getMutex().unlock();

    return file;
}

FileKeepersByPath& BaseFileKeeper::getFilesByPath()
{
    static FileKeepersByPath by_path;
    return by_path;
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
    case BaseFileKeeper::FT_PLAN_VECTOR:
        return BaseFileKeeperPtr( new PlanVectorFileKeeper( path, entity_id ));
    case BaseFileKeeper::FT_PLAN_RASTER:
        return BaseFileKeeperPtr( new PlanRasterFileKeeper( path, entity_id ));
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

PlanFileKeeper::PlanParams PlanFileKeeper::getPlanParams()
{
    return _params;
}

void PlanFileKeeper::setPlanParams(PlanParams params)
{
    _params = params;
}

//----------------------------------------------------

PlanVectorFileKeeper::PlanVectorFileKeeper(QString path, uint64_t entity_id):
    PlanFileKeeper( path, entity_id )
{}

BaseFileKeeper::FileType PlanVectorFileKeeper::getType()
{
    return FT_PLAN_VECTOR;
}

//-----------------------------------------------------

PlanRasterFileKeeper::PlanRasterFileKeeper(QString path, uint64_t entity_id):
    PlanFileKeeper( path, entity_id )
{}

BaseFileKeeper::FileType PlanRasterFileKeeper::getType()
{
    return FT_PLAN_RASTER;
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
