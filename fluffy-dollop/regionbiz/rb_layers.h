#ifndef RB_LAYERS_H
#define RB_LAYERS_H

#include <memory>
#include <set>
#include <mutex>
#include <QString>
#include <QImage>

#include "rb_files.h"
#include "rb_marks.h"

namespace regionbiz
{

class Layer;
typedef std::shared_ptr< Layer > LayerPtr;
typedef std::vector< LayerPtr > LayerPtrs;
typedef std::set< LayerPtr > LayerPtrSet;

class Layer
{
    friend class RegionBizManager;
    friend class BaseDataTranslator;
    friend class LayerManager;
public:
    // params functions
    uint64_t getId();
    QString getName();
    void setName( QString name );
    uint getNumber();
    void setIcon( QImage icon );
    QImage getIcon();

    // metadata
    std::vector< QString > getMetadataNames();
    void addMetadataName( QString name );
    void removeMetadataName( QString name );

    // files
    BaseFileKeeperPtrs getFiles();
    void addFile( QString path );
    void addFile( BaseFileKeeperPtr file );
    void removeFile( QString path );
    void removeFile( BaseFileKeeperPtr file );

    // marks
    MarkPtrs getMarks();
    void addMark( MarkPtr mark );
    void addMark( uint64_t id );
    void removeMark( MarkPtr mark );
    void removeMark( uint64_t id );

    // commit
    bool commit();

    // show/hide
    void show();
    void hide();

    // move in order
    void moveUp();
    void moveDown();
    void moveFirts();
    void moveLast();

private:
    Layer( uint64_t id, QString name );

    // class functions
    static LayerPtr createLayer(QString name);
    static void appendLayer( uint64_t id, QString name );
    static void addLayer( LayerPtr layer );
    static void removeLayer( LayerPtr layer );
    static uint64_t getMaxId();
    static LayerPtrSet& getLayers();
    static std::recursive_mutex &getMutex();

    // params
    uint64_t _id;
    QString _name;
    int _number;
    bool _showed = true;

    // data
    std::set< QString > _files;
    std::set< QString > _metadata;
    std::set< uint64_t > _marks;

    QImage _icon = QImage();
};

//------------------------------------------

class LayerSignalEmiter: public QObject
{
    Q_OBJECT
public:
    LayerSignalEmiter();
    virtual ~LayerSignalEmiter();

    void onLayerChangeShowed( uint64_t id, bool showed );
    void onLayersChangeOrder();
    void onLayerAdded( uint64_t id );
    void onLayersChanged();
    void onLayerDeleted( uint64_t id );

Q_SIGNALS:
    void layerChangeShowed( uint64_t id, bool showed );
    void layersChangeOrder();
    void layerAdded( uint64_t id );
    void layersChanged();
    void layerDeleted( uint64_t id );
};

//------------------------------------------

class LayerManager;
typedef std::shared_ptr< LayerManager > LayerManagerPtr;

class LayerManager
{
    friend class Layer;
    friend class RegionBizManager;
public:
    // manager
    static LayerManagerPtr instance();

    // add
    LayerPtr addLayer( QString name );

    // get
    LayerPtr getLayer( uint64_t id );
    LayerPtrs getLayers();
    LayerPtrs getLayersSortedByNumber();
    std::vector< QString > getLayersNames();
    LayerPtr getLayerByNumber( uint number );
    LayerPtr getLayerByName( QString name );
    uint getLayerCount();

    // move
    void moveLayerUp( LayerPtr layer );
    void moveLayerDown( LayerPtr layer );
    void moveLayerFirst( LayerPtr layer );
    void moveLayerLast( LayerPtr layer );

    // show/hide
    LayerPtrs getLayersByShowed( bool showed );
    LayerPtrs getHiddenLayers();
    LayerPtrs getShowedLayer();
    void showLayer( LayerPtr layer );
    void showLayer( uint64_t id );
    void hideLayer( LayerPtr layer );
    void hideLayer( uint64_t id );

private:
    LayerManager();

    static void normaliseLayersNumbers();

    // subscribes
    void subscribeLayerAdded( QObject* obj,
                              const char *slot,
                              bool queue = false);
    void subscribeLayersChanged( QObject* obj,
                                 const char *slot,
                                 bool queue = false);
    void subscribeLayerDeleted( QObject* obj,
                                const char *slot,
                                bool queue = false);
    void subscribeLayersChangedOrder( QObject* obj,
                                      const char *slot,
                                      bool queue = false);
    void subscribeLayerChangeShowed( QObject* obj,
                                     const char *slot,
                                     bool queue = false);

    LayerSignalEmiter _signal_emiter;
};

}

#endif // RB_LAYERS_H
