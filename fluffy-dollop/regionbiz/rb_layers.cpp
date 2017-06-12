#include "rb_layers.h"

#include <algorithm>

#include "rb_manager.h"

using namespace regionbiz;

uint64_t Layer::getId()
{
    return _id;
}

QString Layer::getName()
{
    return _name;
}

void Layer::setName(QString name)
{
    _name = name;
}

uint Layer::getNumber()
{
    return _number;
}

std::vector< QString > Layer::getMetadataNames()
{
    std::vector< QString > names;
    for( QString name: _metadata )
        names.push_back( name );

    return names;
}

void Layer::addMetadataName(QString name)
{
    auto mngr = RegionBizManager::instance();
    auto layer_prev = mngr->getLayerOfMetadataName( name );
    if( layer_prev )
        layer_prev->removeMetadataName( name );

    _metadata.insert( name );
}

void Layer::removeMetadataName(QString name)
{
    _metadata.erase( name );
}

BaseFileKeeperPtrs Layer::getFiles()
{
    BaseFileKeeperPtrs files;
    auto mngr = RegionBizManager::instance();
    std::vector< QString > for_erase;

    for( QString path: _files )
    {
        BaseFileKeeperPtr file = mngr->getFileByPath( path );
        if( file )
            files.push_back( file );
        else
            for_erase.push_back( path );
    }

    for( QString path: for_erase )
        _files.erase( path );

    return files;
}

void Layer::addFile(QString path)
{
    auto mngr = RegionBizManager::instance();
    auto file = mngr->getFileByPath( path );
    if( file )
    {
        auto layer_prev = mngr->getLayerOfFile( path );
        if( layer_prev )
            layer_prev->removeFile( path );

        _files.insert( path );
    }
}

void Layer::addFile( BaseFileKeeperPtr file )
{
    if( file )
        addFile( file->getPath() );
}

void Layer::removeFile(QString path)
{
    _files.erase( path );
}

void Layer::removeFile(BaseFileKeeperPtr file)
{
    if( file )
        removeFile( file->getPath() );
}

MarkPtrs Layer::getMarks()
{
    MarkPtrs marks;
    auto mngr = RegionBizManager::instance();
    std::vector< uint64_t > for_erase;

    for( uint64_t id: _marks )
    {
        MarkPtr mark = mngr->getMark( id );
        if( mark )
            marks.push_back( mark );
        else
            for_erase.push_back( id );
    }

    for( uint64_t id: for_erase )
        _marks.erase( id );

    return marks;
}

void Layer::addMark(MarkPtr mark)
{
    if( mark )
        addMark( mark->getId() );
}

void Layer::addMark(uint64_t id)
{
    auto mngr = RegionBizManager::instance();
    auto mark = mngr->getMark( id );
    if( mark )
    {
        auto layer_prev = mngr->getLayerOfMark( id );
        if( layer_prev )
            layer_prev->removeMark( id );

        _marks.insert( id );
    }
}

void Layer::removeMark(MarkPtr mark)
{
    if( mark )
        removeMark( mark->getId() );
}

void Layer::removeMark(uint64_t id)
{
    _marks.erase( id );
}

bool Layer::commit()
{
    auto mngr = RegionBizManager::instance();
    bool comm = mngr->commitLayers();

    return comm;
}

void Layer::show()
{
    LayerManager::instance()->showLayer( getId() );
}

void Layer::hide()
{
    LayerManager::instance()->hideLayer( getId() );
}

void Layer::moveUp()
{
    auto layer = LayerManager::instance()->getLayer( getId() );
    LayerManager::instance()->moveLayerUp( layer );
}

void Layer::moveDown()
{
    auto layer = LayerManager::instance()->getLayer( getId() );
    LayerManager::instance()->moveLayerDown( layer );
}

void Layer::moveFirts()
{
    auto layer = LayerManager::instance()->getLayer( getId() );
    LayerManager::instance()->moveLayerFirst( layer );
}

void Layer::moveLast()
{
    auto layer = LayerManager::instance()->getLayer( getId() );
    LayerManager::instance()->moveLayerLast( layer );
}

Layer::Layer(uint64_t id, QString name):
    _id( id ), _name( name )
{
    auto test_layer = LayerManager::instance()->getLayerByName( name );
    if( test_layer )
        std::cerr << "Layer with name " << name.toUtf8().data()
                  << " already exist !!!" << std::endl;
}

LayerPtr Layer::createLayer( QString name )
{
    uint64_t id = getMaxId() + 1;
    LayerPtr new_layer = LayerPtr( new Layer( id, name ));
    addLayer( new_layer );

    return new_layer;
}

void Layer::appendLayer(uint64_t id, QString name)
{
    LayerPtr layer = LayerPtr( new Layer( id, name ));
    addLayer( layer );
}

void Layer::addLayer(LayerPtr layer)
{
    getMutex().lock();
    getLayers().insert( layer );
    getMutex().unlock();

    LayerManager::instance()->moveLayerLast( layer );
    LayerManager::instance()->normaliseLayersNumbers();
}

void Layer::removeLayer(LayerPtr layer)
{
    getMutex().lock();
    getLayers().erase( layer );
    getMutex().unlock();

    LayerManager::normaliseLayersNumbers();
}

uint64_t Layer::getMaxId()
{
    uint64_t max_id = 0;
    getMutex().lock();
    {
        for( LayerPtr layer: getLayers() )
            if( layer->getId() > max_id )
                max_id = layer->getId();
    }
    getMutex().unlock();

    return max_id;

}

LayerPtrSet &Layer::getLayers()
{
    static LayerPtrSet layers;
    return layers;
}

std::recursive_mutex& Layer::getMutex()
{
    static std::recursive_mutex mutex;
    return mutex;
}

//---------------------------------------

LayerManagerPtr LayerManager::instance()
{
    static LayerManagerPtr mngr =
            LayerManagerPtr( new LayerManager );
    return mngr;
}

LayerPtr LayerManager::addLayer(QString name)
{
    return Layer::createLayer( name );
}

LayerPtr LayerManager::getLayer(uint64_t id)
{
    LayerPtr res_layer;

    Layer::getMutex().lock();
    {
    for( LayerPtr layer: Layer::getLayers() )
        if( layer->getId() == id )
        {
            res_layer = layer;
            break;
        }
    }
    Layer::getMutex().unlock();

    return res_layer;
}

LayerPtrs LayerManager::getLayers()
{
    std::vector< LayerPtr > layers;

    Layer::getMutex().lock();
    {
        for( LayerPtr layer: Layer::getLayers() )
            layers.push_back( layer );
    }
    Layer::getMutex().unlock();

    return layers;
}

LayerPtrs LayerManager::getLayersSortedByNumber()
{
    std::vector< LayerPtr > layers = getLayers();

    auto sort_by_num = []( LayerPtr layer_l, LayerPtr layer_r )
    {
        return ( layer_l->getNumber() <= layer_r->getNumber() );
    };
    std::sort( layers.begin(), layers.end(), sort_by_num );

    return layers;
}

std::vector<QString> LayerManager::getLayersNames()
{
    std::vector< QString > names;

    Layer::getMutex().lock();
    {
        for( LayerPtr layer: Layer::getLayers() )
            names.push_back( layer->getName() );
    }
    Layer::getMutex().unlock();

    return names;
}

LayerPtr LayerManager::getLayerByNumber( uint number )
{
    auto layers = getLayersSortedByNumber();
    if( number < layers.size() )
        return layers.at( number );

    return nullptr;
}

LayerPtr LayerManager::getLayerByName(QString name)
{
    LayerPtr res_layer;

    Layer::getMutex().lock();
    {
    for( LayerPtr layer: Layer::getLayers() )
        if( layer->getName() == name )
        {
            res_layer = layer;
            break;
        }
    }
    Layer::getMutex().unlock();

    return res_layer;
}

uint LayerManager::getLayerCount()
{
    uint count;
    Layer::getMutex().lock();
    {
        count = Layer::getLayers().size();
    }
    Layer::getMutex().unlock();
    return count;
}

void LayerManager::moveLayerUp( LayerPtr layer )
{
    normaliseLayersNumbers();

    uint this_number = layer->getNumber();
    if( 0 == this_number )
        return;

    auto prev_layer = getLayerByNumber( this_number - 1 );
    prev_layer->_number = this_number;
    layer->_number = this_number - 1;

    _signal_emiter.onLayersChangeOrder();
}

void LayerManager::moveLayerDown(LayerPtr layer)
{
    normaliseLayersNumbers();

    uint this_number = layer->getNumber();
    if( 0 == getLayerCount() )
        return;

    auto next_layer = getLayerByNumber( this_number + 1 );
    next_layer->_number = this_number;
    layer->_number = this_number + 1;

    _signal_emiter.onLayersChangeOrder();
}

void LayerManager::moveLayerFirst(LayerPtr layer)
{
    layer->_number = -1;
    normaliseLayersNumbers();

    _signal_emiter.onLayersChangeOrder();
}

void LayerManager::moveLayerLast(LayerPtr layer)
{
    layer->_number = getLayerCount();
    normaliseLayersNumbers();

    _signal_emiter.onLayersChangeOrder();
}

LayerPtrs LayerManager::getLayersByShowed( bool showed )
{
    LayerPtrs layers;
    for( LayerPtr layer: getLayers() )
    {
        bool filtered = ( showed && layer->_showed )
                || ( !showed && !layer->_showed );

        if( filtered )
            layers.push_back( layer );
    }

    return layers;
}

LayerPtrs LayerManager::getShowedLayer()
{
    bool showed( true );
    return getLayersByShowed( showed );
}

LayerPtrs LayerManager::getHiddenLayers()
{
    bool showed( true );
    return getLayersByShowed( !showed );
}

void LayerManager::showLayer(LayerPtr layer)
{
    if( !layer->_showed )
    {
        layer->_showed = true;
        _signal_emiter.onLayerChangeShowed( layer->getId(), true );
    }
}

void LayerManager::showLayer(uint64_t id)
{
    LayerPtr layer = getLayer( id );
    if( layer )
        showLayer( layer );
}

void LayerManager::hideLayer(LayerPtr layer)
{
    if( layer->_showed )
    {
        layer->_showed = false;
        _signal_emiter.onLayerChangeShowed( layer->getId(), false );
    }
}

void LayerManager::hideLayer(uint64_t id)
{
    LayerPtr layer = getLayer( id );
    if( layer )
        hideLayer( layer );
}

void LayerManager::subscribeLayerAdded( QObject *obj,
                                        const char *slot,
                                        bool queue )
{
    QObject::connect( &_signal_emiter, SIGNAL( layerAdded( uint64_t )),
                      obj, slot,
                      queue ? Qt::QueuedConnection : Qt::DirectConnection );
}

void LayerManager::subscribeLayersChanged(QObject *obj,
                                          const char *slot,
                                          bool queue)
{
    QObject::connect( &_signal_emiter, SIGNAL( layersChanged() ),
                      obj, slot,
                      queue ? Qt::QueuedConnection : Qt::DirectConnection );
}

void LayerManager::subscribeLayerDeleted(QObject *obj,
                                         const char *slot,
                                         bool queue)
{
    QObject::connect( &_signal_emiter, SIGNAL( layerDeleted( uint64_t )),
                      obj, slot,
                      queue ? Qt::QueuedConnection : Qt::DirectConnection );
}

void LayerManager::subscribeLayersChangedOrder(QObject *obj,
                                               const char *slot,
                                               bool queue)
{

    QObject::connect( &_signal_emiter, SIGNAL( layersChangeOrder()),
                      obj, slot,
                      queue ? Qt::QueuedConnection : Qt::DirectConnection );
}

void LayerManager::subscribeLayerChangeShowed(QObject *obj,
                                              const char *slot,
                                              bool queue)
{

    QObject::connect( &_signal_emiter, SIGNAL( layerChangeShowed( uint64_t, bool )),
                      obj, slot,
                      queue ? Qt::QueuedConnection : Qt::DirectConnection );
}

LayerManager::LayerManager()
{}

void LayerManager::normaliseLayersNumbers()
{
    LayerPtrs layers = instance()->getLayersSortedByNumber();
    for( uint i = 0; i < layers.size(); ++i )
        layers[ i ]->_number = i;
}

//------------------------------------------------------

LayerSignalEmiter::LayerSignalEmiter()
{}

LayerSignalEmiter::~LayerSignalEmiter()
{}

void LayerSignalEmiter::onLayerChangeShowed(uint64_t id, bool showed)
{
    Q_EMIT layerChangeShowed( id, showed );
}

void LayerSignalEmiter::onLayersChangeOrder()
{
    Q_EMIT layersChangeOrder();
}

void LayerSignalEmiter::onLayerAdded(uint64_t id)
{
    Q_EMIT layerAdded( id );
}

void LayerSignalEmiter::onLayersChanged()
{
    Q_EMIT layersChanged();
}

void LayerSignalEmiter::onLayerDeleted(uint64_t id)
{
    Q_EMIT layerDeleted( id );
}
