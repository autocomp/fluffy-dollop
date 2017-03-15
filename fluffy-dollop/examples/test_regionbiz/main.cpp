#include <QDebug>

#include <regionbiz/rb_manager.h>

#include "test_reciver.h"

int main()
{
    using namespace regionbiz;

    // test destructor
    Location* loc = new Location(1);
    delete loc;

    //! init
    auto mngr = RegionBizManager::instance();
    QString str = "~/.contour_ng/regionbiz_sqlite.json";
    //QString str = "~/.contour_ng/regionbiz_psql.json";
    mngr->init( str );

    //! load regons
    std::vector< RegionPtr > regions = mngr->getRegions();
    //! get first region
    RegionPtr reg = regions.front();
    //! print region info
    qDebug() << "Region info: " << reg->getDescription();
    for( QPointF coord: reg->getCoords() )
        qDebug() << "  Point reg: " << coord.x() << "x" << coord.y();

    //! get child locations (without facilitys)
    std::vector< BaseAreaPtr > locations = reg->getChilds( Region::RCF_LOCATIONS );
    //std::vector< LocationPtr > locations = mngr->getLocationsByParent( reg->getId() );

    //! print locations info
    for( BaseAreaPtr ptr: locations )
    {
        LocationPtr loc = BaseArea::convert< Location >( ptr );
        qDebug() << " New location: " << loc->getAddress() << ", " << loc->getDescription()
                  << " (" << loc->getPlanPath() << ")";

        PlanKeeper::PlanParams params = loc->getPlanParams();
        qDebug() << "  Params of plan: " << params.scale_h << "x" << params.scale_w
                  << ", " << params.rotate << ", " << params.x << "x" << params.y;

        for( QPointF coord: loc->getCoords() )
            qDebug() << "   Location Point: " << coord.x() << "x" << coord.y();
    }

    //! get child facilitys (with chllds of all child locations)
    std::vector< BaseAreaPtr > facilitys = reg->getChilds( Region::RCF_ALL_FACILITYS );
    // std::vector< FacilityPtr > facilitys = mngr->getFacilitysByParent( reg->getId() );
    // for( BaseLocationPtr loc: locations )
    // {
    //     auto facils = mngr->getFacilitysByParent( loc->getId() );
    //     for( FacilityPtr fac: facils )
    //         facilitys.push_back( fac );
    // }

    //! print facilitys info
    for( BaseAreaPtr fac_ptr: facilitys )
    {
        FacilityPtr fac = BaseArea::convert< Facility >( fac_ptr );
        qDebug() << " New Facility: " << fac->getDescription() << ", " << fac->getAddress() << ", "
                  << fac->getCadastralNumber();

        for( QPointF coord: fac->getCoords() )
            qDebug() << "  Facility Point: " << coord.x() << "x" << coord.y();
    }

    //! process floors
    for( BaseAreaPtr fac_ptr: facilitys )
    {
        //! load floors
        FacilityPtr fac = BaseArea::convert< Facility >( fac_ptr );
        FloorPtrs floors = fac->getChilds();
        // FlorsPtrs floors = mngr->getFloorsByParent( fac->getId() );

        for( FloorPtr flo: floors )
        {
            //! print info
            qDebug() << "  Floor info: " << flo->getNumber() << ": " << flo->getName() << ", "
                      << ( flo->getCoords().length() ? "have coords" : "don't have coords" );

            //! process rooms
            BaseAreaPtrs rooms = flo->getChilds( Floor::FCF_ALL_ROOMS );
            for( BaseAreaPtr room_ptr: rooms )
            {
                RoomPtr room = BaseArea::convert< Room >( room_ptr );

                //! print info
                qDebug() << "    Room: " << room->getName() << ", "
                          << ( room->getCoords().length() ? "have coords" : "don't have coords" );
            }
        }
    }

    //! Selection test
    TestReciver recv;
    mngr->subscribeOnSelect( &recv, SLOT(onSelection(uint64_t,uint64_t)) );
    mngr->selectArea( 4 );
    mngr->selectArea( 5 );

    mngr->subscribeCenterOn( &recv, SLOT(onCenterOn(uint64_t)) );
    mngr->centerOnArea( 6 );

    //! test of metadate
    auto area = mngr->getBaseArea( 13 );
    auto metadata = area->getMetadataMap();
    qDebug() << "Meta:" << metadata.size();
    for( auto data_pair: metadata )
    {
        BaseMetadataPtr data = data_pair.second;

        qDebug() << "  Data:" << data->getName() << "-" << data->getValueAsString();
        DoubleMetadataPtr double_data = BaseMetadata::convert< DoubleMetadata >( data );
        if( double_data )
            qDebug() << "  Real val:" << double_data->getValue();
    }
    qDebug() << " Data square:" << area->getMetadata( "square" )->getValueAsString();

    //! test of entitys
    qDebug() << "Max id =" << BaseEntity::getMaxId();
    // create room
    auto room = mngr->addArea< Room >( 13 );
    qDebug() << "Max id after =" << BaseEntity::getMaxId();
    // change room
    room->setCoords(( QPolygonF() << QPointF( 1, 1 ) << QPointF( 1, 100 )
                      << QPointF( 100, 100 ) << QPointF( 100, 1 )));
    BaseArea::convert< Room >( room )->setName( QString::fromUtf8( "Тестовое имя" ));
    // commit room (change base)
    //qDebug() << "Commit:" << room->commit();
    //qDebug() << "Commit again:" << room->commit();
}
