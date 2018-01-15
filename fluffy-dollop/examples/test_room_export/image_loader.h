#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <iostream>
#include <QObject>
#include <QTimer>
#include <QFileInfo>
#include <QDebug>

#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>

class ImageLoader: public QObject
{
    Q_OBJECT
public:
    void loadFileForFloor( uint64_t floor_id )
    {
        using namespace std;
        using namespace regionbiz;

        _floor_id = floor_id;
        QTimer* timer = new QTimer( this );
        timer->singleShot( 10000, this, SLOT( onTimerout() ));
    }

    void onPlanLoaded( regionbiz::BaseFileKeeperPtr plan )
    {
        using namespace std;
        using namespace regionbiz;
        auto mngr = RegionBizManager::instance();

        QFileInfo info( *(plan->getLocalFile()) );
        cout << "Plan location: " << info.absoluteFilePath().toUtf8().data() << endl;
        cout << "Size: " << info.size() << endl;


        auto floor = mngr->getBaseArea( _floor_id )->convert< Floor >();
        FacilityPtr facility = floor->getParent()->convert< Facility >();

        // get transform
        auto transform = facility->getTransform();
        qDebug() << "Transform:" << transform;

        QImage image( info.absoluteFilePath() );
        QPolygonF mapped_rect = transform.map( QPolygon( image.rect() ));
        for( QPointF pnt: mapped_rect )
            cout << " x: " << pnt.x() << ", y: " << pnt.y() << endl;
    }

private Q_SLOTS:
    void onTimerout()
    {
        using namespace std;
        using namespace regionbiz;
        auto mngr = RegionBizManager::instance();

        // check floor
        auto floor = mngr->getBaseArea( _floor_id )->convert< Floor >();
        if( !floor )
        {
            cerr << "It's not a floor" << endl;
            return;
        }

        // get plan
        FacilityPtr facility = floor->getParent()->convert< Facility >();
        auto plan = facility->getEtalonPlan();
        cout << "State " << plan->getFileState() << endl;
        if( BaseFileKeeper::FS_UNSYNC == plan->getFileState() )
        {
            cout << "Start Sync" << endl;
            mngr->subscribeFileSynced( this, SLOT( onSync( regionbiz::BaseFileKeeperPtr )));
            plan->syncFile();
        }
        else if( BaseFileKeeper::FS_SYNC == plan->getFileState() )
            onPlanLoaded( plan );
        else
            cerr << "Invalid state of plan" << endl;
    }

    void onSync( regionbiz::BaseFileKeeperPtr file )
    {
        using namespace std;
        cout << "File sync: " << file->getId().toUtf8().data() << endl;

        onPlanLoaded( file );
    }

private:
    uint64_t _floor_id;
};

#endif // IMAGE_LOADER_H
